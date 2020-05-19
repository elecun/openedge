
/**
 * @file    PLC Data Aqu. Task
 * @brief   Data aquisition task with PLC, XGT protocol via FEnet
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_PLCDAQ_TASK_HPP_
#define _OPENEDGE_PLCDAQ_TASK_HPP_

#include <openedge/core/task.hpp>
#include <openedge/core/service.hpp>
#include <map>

using namespace oe;

class plcDaqTask : public core::task::runnable {
    public:
        plcDaqTask() = default;
        virtual ~plcDaqTask() = default;

        //common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;

    private:
        bool loadService(const char* servicename);
        void unloadService();
};

EXPORT_RT_TASK_API

#endif