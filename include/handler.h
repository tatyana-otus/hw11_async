#pragma once

#include "consts_types.h"

struct Handler
{
    void stream_out(const p_data_type v, std::ostream& os)
    {   
        if(!v->empty()){
        
            ++blk_count;

            os << "bulk: " << *(v->cbegin());
            ++cmd_count;
            for (auto it = std::next(v->cbegin()); it != std::cend(*v); ++it){
                os << ", " << *it ;
                ++cmd_count;   
            }
            os.flush();           
        }
        else {
            throw std::invalid_argument("Empty bulk !");
        }
    }


    void print_metrics(std::ostream& os = std::cout) const
    {
        os << cmd_count << " commands, "
           << blk_count << " blocks" 
           <<  "\n";
    }


    void stop (void) 
    {
        helper_thread.join();
    } 


    virtual void start(void) = 0; 


    std::thread helper_thread;

    size_t blk_count = 0;
    size_t cmd_count = 0;
    
    std::atomic<bool> quit{false};

    std::atomic<bool> failure{false};
    std::exception_ptr eptr;
};
