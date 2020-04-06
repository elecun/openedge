
/**
 * @file    simple.task.hpp
 * @brief   simple message task for example
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_TASK_SIMPLE_HPP_
#define _OPENEDGE_TASK_SIMPLE_HPP_

#include <openedge/core/task.hpp>

namespace oe {
    namespace task {

        class simpletask : public oe::core::rt_task::runnable {
            public:
            bool configure();
            void execute();
            void cleanup();

            private:

        };

    } //namepspace task
} //namespace oe

#endif