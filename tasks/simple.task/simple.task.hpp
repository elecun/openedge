
/**
 * @file    simple.task.hpp
 * @brief   simple message task for example
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_TASK_SIMPLE_HPP_
#define _OPENEDGE_TASK_SIMPLE_HPP_

#include <openedge/core/task.hpp>
#include <time.h>

using namespace oe;

class simpletask : public oe::core::task::runnable {
    public:
        simpletask() = default;
        virtual ~simpletask() = default;

        //common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;

    private:
        struct timespec x; 
};

EXPORT_TASK_API

#endif