
/**
 * @file    Data Aquisiiton Task form PLC
 * @brief   
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_PLCDAQ_TASK_HPP_
#define _OPENEDGE_PLCDAQ_TASK_HPP_

#include <openedge/core/task.hpp>
#include <time.h>

class plcDaqTask : public oe::core::rt_task::runnable {
    public:
    ~plcDaqTask();
    bool configure();
    void execute();
    void cleanup();

    private:
    struct timespec x; 
};

EXPORT_RT_TASK_API

#endif