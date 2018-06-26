#include <iostream>
#include <algorithm>
#include <ctime>
#include <vector>
#include <thread>
#include <atomic>
#include <sstream>

#include "consts_types.h"


struct Command {

    Command(size_t N_, std::shared_ptr<f_tasks_t> f_task_, std::shared_ptr<p_tasks_t> print_task_, 
    size_t context_id_):
    N(N_), file_task(f_task_), print_task(print_task_), context_id(context_id_),braces_count(0),
    idx_write(0), file_id(0)
    {
        cur_data = std::make_shared<data_type>();
        cur_data->reserve(N);
    }

    std::mutex get_data_mx;
    
    void set_bulk_size(size_t bulk_size)
    {
        N = bulk_size;
    }

    void on_new_cmd(const std::string& d)
    {
        BulkState blk_state = BulkState::save;

        if(d == "{") {  
            if(braces_count == 0) {
                blk_state = BulkState::end;
            }
            ++braces_count;        
        }
        else if (d == "}"){
            --braces_count;
            if(braces_count == 0) {
                blk_state = BulkState::end;
            }
            else if(braces_count < 0){
                braces_count = 0;
                cur_data->clear();
                throw std::invalid_argument("wrong command stream");
            }    
        }
        else {
            if(cur_data->empty())
                init_time = std::time(nullptr);
            cur_data->push_back(d);
        }

        exec_state(blk_state);
    }


    void on_cmd_end()
    {
        if(braces_count == 0){
            exec_state(BulkState::end);
        }
        else{
            cur_data->clear(); 
            braces_count = 0;   
        }
    }

    void get_data(const std::string& line)
    { 
        if(line.size() > MAX_CMD_LENGTH){
            std::string msg = "Invalid command length. Command length must be < " 
                              + std::to_string(MAX_CMD_LENGTH);
            throw std::invalid_argument(msg.c_str());
        }
        on_new_cmd(line);      
    }

private:
    void on_bulk_created()
    { 
        if(!cur_data->empty()) { 

            set_logger_task();
            set_printer_task();

            update_write_index();
        }       
    }


    void set_logger_task()
    {
        std::unique_lock<std::mutex> lk_file(file_task->cv_mx);

        if(file_task->size() > MAX_QUEUE_SIZE) {
            file_task->cv.notify_one();
            file_task->cv_empty.wait(lk_file, [this](){ 
            return( this->file_task->size() <  MAX_QUEUE_SIZE); });
        }

        file_task->push(std::make_tuple(cur_data, init_time, (context_id << 32) | (++file_id)));
        lk_file.unlock();
        file_task->cv.notify_one();
    }


    void set_printer_task()
    {
        std::unique_lock<std::mutex> lk(print_task->cv_mx);
        
        if(print_task->size() > MAX_QUEUE_SIZE) {
            print_task->cv.notify_one();
            print_task->cv_empty.wait(lk, [this](){ 
            return( this->print_task->size() <  MAX_QUEUE_SIZE); });
        }
        
        print_task->push( cur_data );
        lk.unlock();
        print_task->cv.notify_one();
    }



    void update_write_index()
    {
        cur_data = nullptr;
        cur_data = std::make_shared<data_type>();
        cur_data->reserve(N);
    }

    
    enum class BulkState { end, save };

    
    void exec_state(BulkState state) {

        switch(state) {

            case BulkState::end:
                on_bulk_created();
                break;

            case BulkState::save:
                if((braces_count == 0) && (cur_data->size() == N)){
                    exec_state(BulkState::end);
                }
                break;
        }
    }

private:
    size_t N;

    std::shared_ptr<f_tasks_t> file_task;
    std::shared_ptr<p_tasks_t> print_task;

    std::time_t init_time;

    std::shared_ptr<data_type> cur_data;
    
    size_t context_id;
    int braces_count; 
    size_t idx_write;
    size_t file_id;  
};