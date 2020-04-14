
/**
 * @file    lsisFenet.task.hpp
 * @brief   LSIS Fast Ethernet task
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_TASK_SIMPLE_HPP_
#define _OPENEDGE_TASK_SIMPLE_HPP_

#include <openedge/core/task.hpp>
#include <time.h>

class simpletask : public oe::core::rt_task::runnable {
    public:
    ~simpletask();
    bool configure();
    void execute();
    void cleanup();

    private:
    struct timespec x; 
};

EXPORT_RT_TASK_API

#endif