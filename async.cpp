#include "async.h"
#include "application.h"

Application app;

namespace async {

    handle_t connect(std::size_t bulk)
    {
        if(app.is_fail()){
            std::cerr << app.fail_msg << std::endl;
            return nullptr;
        }

        try{
            if((bulk == 0) || (bulk > MAX_BULK_SIZE)){   
                return nullptr;    
            }

            auto index = app.connect(bulk);

            return reinterpret_cast<void*>(index);
        }
        catch(const std::exception &e) {
            std::cerr << e.what() << std::endl;
            app.set_fail();
            return nullptr;
        }    
    }


    void receive(handle_t handle, const char *data, std::size_t size)
    {
        if(app.is_fail()){
            std::cerr << app.fail_msg << std::endl;
            return;
        }

        try{
            app.receive(reinterpret_cast<size_t>(handle), data, size);
        }    
        catch(const std::exception &e) {
            std::cerr << e.what() << std::endl;
            app.set_fail();
        }
    }


    void disconnect(handle_t handle)
    {
        if(app.is_fail()){
            std::cerr << app.fail_msg << std::endl;
            return;
        }
        try{
            app.disconnect(reinterpret_cast<size_t>(handle));
        }    
        catch(const std::exception &e) {
            std::cerr << e.what() << std::endl;
            app.set_fail();
        }
    }
}