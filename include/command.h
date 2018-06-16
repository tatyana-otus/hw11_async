#include <iostream>
#include <algorithm>
#include <ctime>
#include <vector>
#include <memory>
#include <condition_variable>
#include <queue>
#include <thread>
#include <atomic>
#include <exception>
#include <sstream>

#include "consts_types.h"


struct Command {

    Command(size_t N_, p_file_tasks f_task_,p_print_task print_task_, size_t context_id_):
    N(N_), file_task(f_task_), print_task(print_task_), context_id(context_id_),braces_count(0),
    idx_write(0), file_id(0)
    {
        for(size_t i = 0; i < CIRCLE_BUFF_SIZE; ++i) {

            data[i].reserve(MAX_BULK_SIZE);
            data_ext[i] = false;
        }
    }


    void set_bulk_size(size_t bulk_size)
    {
        N = bulk_size;
    }


    void on_bulk_created()
    {
        if(!data[idx_write].empty()) {   

            data_ext[idx_write] = true;    //no need for lock

            set_logger_task();
            set_printer_task();

            update_write_index();
        }       
    }


    void set_logger_task()
    {
        std::unique_lock<std::mutex> lk_file(file_task->cv_mx);
        file_task->push(std::make_tuple(std::make_tuple(&data[idx_write], 
                                                       init_time, 
                                                       (context_id << 32) | (++file_id)
                                                       ), 
                                       &data_ext[idx_write]) 
                                       );
        lk_file.unlock();
        file_task->cv.notify_one();
    }


    void set_printer_task()
    {
        std::unique_lock<std::mutex> lk(print_task->cv_mx);
        

        if(print_task->size() > (CIRCLE_BUFF_SIZE - 3)) {
            print_task->cv.notify_one();
            print_task->cv_empty.wait(lk, [this](){ 
            return( this->print_task->size() <  (CIRCLE_BUFF_SIZE / 2)); });
        }
        
        print_task->push( &data[idx_write] );

        lk.unlock();

        print_task->cv.notify_one();
    }


    void update_write_index()
    {
        ++idx_write;
        idx_write %= CIRCLE_BUFF_SIZE;
       // ++blk_count;

        std::unique_lock<std::mutex> lk_ext_data(file_task->cv_mx_empty);
        if(data_ext[idx_write] == true) { 
            file_task->cv.notify_one();
            file_task->cv_empty.wait(lk_ext_data, [this](){ return !this->data_ext[idx_write]; });
        }
        lk_ext_data.unlock(); 
    }

    
    enum class BulkState { end, save };

    void on_new_cmd(const std::string& d)
    {
        BulkState blk_state = BulkState::save;

        //++str_count;

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
                throw std::invalid_argument("wrong command stream");
            }    
        }
        else {
            if(data[idx_write].empty())
                init_time = std::time(nullptr);
            data[idx_write].push_back(d);
            //++cmd_count;
        }

        exec_state(blk_state);
    }


    void on_cmd_end()
    {
        if(braces_count == 0){
            exec_state(BulkState::end);
        }
    }


    void exec_state(BulkState state) {

        switch(state) {

            case BulkState::end:
                on_bulk_created();
                data[idx_write].clear();
                break;

            case BulkState::save:
                if((braces_count == 0) && (data[idx_write].size() == N)){
                    exec_state(BulkState::end);
                }
                break;
        }
    }


    void get_data(std::string line)
    {
        try {  
            if(line.size() > MAX_CMD_LENGTH){
                std::string msg = "Invalid command length. Command length must be < " 
                                  + std::to_string(MAX_CMD_LENGTH) + " :" + line + ".\n";
                throw std::invalid_argument(msg.c_str());
            }
            on_new_cmd(line);
        }
        catch(const std::exception &e) {
            throw;
        }
    }


private:
    size_t N;
    p_file_tasks file_task;
    p_print_task print_task;

    std::time_t init_time;

    std::array<data_type, CIRCLE_BUFF_SIZE> data;
    std::array<bool,      CIRCLE_BUFF_SIZE> data_ext;
    
    size_t context_id;
    int braces_count; 
    size_t idx_write;
    size_t file_id;
};