
/**
 * @file    simple.task.hpp
 * @brief   simple message task for example
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_TASK_SIMPLE2_HPP_
#define _OPENEDGE_TASK_SIMPLE2_HPP_

#include <openedge/core/task.hpp>
#include <time.h>

class simpletask2 : public oe::core::task::runnable {
    public:
    ~simpletask2();
    bool configure();
    void execute();
    void cleanup();

    private:
    struct timespec x; 
};

EXPORT_TASK_API

#endif