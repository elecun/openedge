
/**
 * @file    simple.task.hpp
 * @brief   simple message task for example
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_TASK_SYSMON_HPP_
#define _OPENEDGE_TASK_SYSMON_HPP_

#include <openedge/core/task.hpp>
#include <time.h>

class  sysmonTask : public oe::core::rt_task::runnable {
    public:
    ~sysmonTask();
    bool configure();
    void execute();
    void cleanup();

    private:
    struct timespec x; 
};

EXPORT_RT_TASK_API

#endif