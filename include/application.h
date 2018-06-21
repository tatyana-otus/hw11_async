#include <functional>
#include <map>
#include <tuple>
#include <shared_mutex>
#include "command.h"
#include "bulk_handlers.h"

struct Application
{	
    using context_index_t = std::size_t;
    using context_data_t  = std::tuple<std::string, std::unique_ptr<Command>, bool>;
    using context_t       = std::map<context_index_t, 
                                     context_data_t, 
                                     std::greater<context_index_t>
                                    >;

    Application(std::ostream& os = std::cout, 
                size_t file_th_cnt_ = 2):file_th_cnt(file_th_cnt_)
    {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        auto main_th_id = ss.str();

        q_file  = std::make_shared<queue_wrapper<f_msg_type_ext>>();
        q_print = std::make_shared<queue_wrapper<p_data_type>>();

        data_log = std::make_shared<PrintData>(q_print, os); 
        file_log.resize(file_th_cnt);
        for(size_t i = 0; i < file_th_cnt; ++i){
            file_log[i] = std::make_shared<WriteData>(q_file, 
                                                      main_th_id);
        }

        start();
    }


    ~Application()
    {
        try {  
            stop();
        }
        catch(const std::exception &e) {

        }       
    }


    context_index_t connect(size_t bulk)
    {   
        if((bulk == 0) || (bulk > MAX_BULK_SIZE)){       
            throw std::invalid_argument("Invalid block size.");
        }

        std::unique_lock<std::shared_timed_mutex> lock(context_mx);

        auto index = add_context(bulk);

        return index;
    }


    void receive(context_index_t handle, const char *data, std::size_t size)
    {
        std::shared_lock<std::shared_timed_mutex> lock(context_mx);

        transmit_data(handle, data, size); 
    }


    void disconnect(context_index_t handle)
    {
        std::unique_lock<std::shared_timed_mutex> lock(context_mx);

        remove_context(handle);
    }

protected:

    void start()
    {
        data_log->start();
        for (auto const& f : file_log) {
            f->start();
        }
    }


    void stop()
    {
        data_log->quit = true;
        for (auto & f : file_log) {
            f->quit = true;
        }

        std::unique_lock<std::mutex> lk(q_print->cv_mx);
        q_print->cv.notify_one();
        lk.unlock();

        std::unique_lock<std::mutex> lk_file(q_file->cv_mx);
        q_file->cv.notify_all();
        lk_file.unlock();

        data_log->stop();
        for (auto const& f : file_log) {
            f->stop();
        }
    }


    context_index_t add_context(std::size_t bulk_size)
    {
        auto it_unused_context = std::find_if(context.rbegin(), 
                                              context.rend(), 
                                              [](const auto& val)
                                              {
                                                 return (std::get<2>(val.second) == false);
                                              });

        if(it_unused_context != context.rend()) {

            std::get<0>(it_unused_context->second) = "";
            std::get<1>(it_unused_context->second)->set_bulk_size(bulk_size);
            std::get<2>(it_unused_context->second) = true;

            return it_unused_context->first;
        }
        else { 

            context_index_t index = 0;

            if(!context.empty()){
                index = context.begin()->first;
            }
            ++index;

            context[index] = std::make_tuple("", 
                                            std::make_unique<Command>(bulk_size, 
                                                                      q_file, 
                                                                      q_print, 
                                                                      index),
                                            true);

            std::get<0>(context[index]).reserve(MAX_CMD_LENGTH);

            return index;
        }      
    }


    void remove_context(context_index_t index)
    {   
        auto it = context.find(index);

        if(it != context.end()) {
            if(std::get<2>(it->second) == true){
                std::get<1>(it->second)->on_cmd_end();
                std::get<2>(it->second) = false;
                std::get<0>(it->second) = "";
            }            
        }      
    }


    void transmit_data(context_index_t index, const char *data, std::size_t size)
    {
        auto it = context.find(index);
        if(it != context.end()) {
            if(std::get<2>(it->second) == true){

                std::lock_guard<std::mutex> lock(std::get<1>(it->second)->get_data_mx);

                for(size_t i = 0; i < size; ++i){
                    if(data[i] != '\n'){
                        std::get<0>(it->second) += data[i];
                    }
                    else{
                        if(check_falure()) throw std::runtime_error("Some worker thread terminated due to some exception.");
                        try {
                            std::get<1>(it->second)->get_data(std::get<0>(it->second));
                        }
                        catch(const std::exception& e)
                        {
                            std::get<0>(it->second) = "";
                            throw;
                        }    
                        std::get<0>(it->second) = "";
                    }
                }
            }       
        }
    }


    bool check_falure() const
    {
        if(data_log->failure) return true;

        for (auto const& f : file_log) {
            if(f->failure) return true;
        }
        return false;
    }


    /*void handle_first_th_exeption() const
    {
        if(data_log->eptr) return std::rethrow_exception(data_log->eptr);

        for (auto const& f : file_log) {
            if (f->eptr) {
                std::rethrow_exception(f->eptr);
            }
        }
    }*/

    size_t file_th_cnt;

    context_t  context; 
    std::shared_timed_mutex context_mx;

    std::shared_ptr<PrintData> data_log;
    std::vector<std::shared_ptr<WriteData>> file_log;

    std::shared_ptr<queue_wrapper<f_msg_type_ext>> q_file;
    std::shared_ptr<queue_wrapper<p_data_type>>    q_print;
};