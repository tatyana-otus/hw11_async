#include "async.h"
#include "application.h"

Application app;

namespace async {

    handle_t connect(std::size_t bulk)
    {
        auto index = app.connect(bulk);

        return reinterpret_cast<void*>(index);
    }


    void receive(handle_t handle, const char *data, std::size_t size)
    {
        app.receive(reinterpret_cast<size_t>(handle), data, size);
    }


    void disconnect(handle_t handle)
    {
        app.disconnect(reinterpret_cast<size_t>(handle));
    }
}