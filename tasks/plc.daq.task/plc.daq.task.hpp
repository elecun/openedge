
/**
 * @file    Data Aquisiiton Task
 * @brief   use general PLC which has FEnet I/F
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_PLCDAQ_TASK_HPP_
#define _OPENEDGE_PLCDAQ_TASK_HPP_

#include <openedge/core/task.hpp>
#include <openedge/core/bus.hpp>
#include <openedge/core/device.hpp>

#include <time.h>

class plcDaqTask : public oe::core::rt_task::runnable {
    public:
    plcDaqTask() = default;
    ~plcDaqTask();

    bool configure();
    void execute();
    void cleanup();

    private:
    struct timespec x; 
    oe::core::ibusAsync* _bus = nullptr;
    oe::core::idevice* _plc = nullptr;
};

EXPORT_RT_TASK_API

#endif