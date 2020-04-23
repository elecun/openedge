
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

using namespace oe;

class plcDaqTask : public core::rt_task::runnable {
    public:
        plcDaqTask() = default;
        virtual ~plcDaqTask();

        bool configure() override;
        void execute() override;
        void cleanup() override;

    private:
        bool loadService(const char* servicename);
        void unloadService();

    private:
        void* _plcServiceHandle = nullptr;

    private:
        map<string, void*> _srvHandleContainer;
        map<string, core::iService*> _srvContainer;
};

EXPORT_RT_TASK_API

#endif