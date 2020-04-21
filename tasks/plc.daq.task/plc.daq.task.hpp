
/**
 * @file    PLC Data Aqu. Task
 * @brief   use general PLC which has FEnet I/F
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_PLCDAQ_TASK_HPP_
#define _OPENEDGE_PLCDAQ_TASK_HPP_

#include <openedge/core/task.hpp>
#include <openedge/core/bus.hpp>
#include <openedge/core/device.hpp>
#include <openedge/core/service.hpp>
#include "lsis_plc.hpp"

class plcDaqTask : public oe::core::rt_task::runnable {
    public:
        plcDaqTask() = default;
        ~plcDaqTask();

        //RT Task Common Interface
        bool configure() override;
        void execute() override;
        void cleanup() override;

    private:
        fenet* _bus = nullptr;
        lsisPlc* _plc = nullptr;
        //oe::core::iService* _plc = nullptr; //plc service (data collection)
        //oe::core::iService* _bus = nullptr; //bus interface connected to PLC
};

EXPORT_RT_TASK_API

#endif