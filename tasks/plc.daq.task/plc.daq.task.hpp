
/**
 * @file    PLC Data Aqu. Task
 * @brief   use general PLC which has FEnet I/F
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_PLCDAQ_TASK_HPP_
#define _OPENEDGE_PLCDAQ_TASK_HPP_

#include <openedge/core/task.hpp>
#include <openedge/core/service.hpp>
#include <map>
#include <tuple>

using namespace oe;

class plcDaqTask : public core::rt_task::runnable {
    public:
        plcDaqTask() = default;
        ~plcDaqTask();

        //RT Task Common Interface
        bool configure() override;
        void execute() override;
        void cleanup() override;

    private:
        //bool loadService(const char* servicename, void* handle, core::iServiceBase* pImpl);
        //void releaseService();

    private:
        //map<int, pair<void*, core::iServiceBase*>> _serviceContainer;

    private:
        //core::iService<core::iDeviceExtendable<int, device::PLC*>>* _plc = nullptr; //plc service (data collection)
        //core::iService<device::iPLC*>* _plc = nullptr; //plc service (data collection)
        //core::iService<core::bus::iDeviceBus*>* _bus = nullptr; //bus interface connected to PLC
};

EXPORT_RT_TASK_API

#endif