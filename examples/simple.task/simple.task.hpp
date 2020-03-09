

#ifndef _OPENEDGE_EXAMPLE_SIMPLETASK_HPP_
#define _OPENEDGE_EXAMPLE_SIMPLETASK_HPP_

#include <openedge/core/task.hpp>

class simpletask : public oe::core::task {
    public:
        simpletask();
        ~simpletask();

        bool configure();
        void execute();
        void cleanup();
};

#endif