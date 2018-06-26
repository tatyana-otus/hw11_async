#include "consts_types.h"

struct Handler
{
    void stop (void) 
    {
        helper_thread.join();
    } 


    virtual void start(void) = 0; 

    std::atomic<bool> quit{false};

    std::atomic<bool> failure{false};
    std::exception_ptr eptr;

protected:

    void stream_out(const p_data_type v, std::ostream& os)
    {   
        if(!v->empty()){
        
            os << "bulk: " << *(v->cbegin());
            for (auto it = std::next(v->cbegin()); it != std::cend(*v); ++it){
                os << ", " << *it ; 
            }
            os.flush();           
        }
        else {
            throw std::invalid_argument("Empty bulk !");
        }
    }
    
    std::thread helper_thread;
};
