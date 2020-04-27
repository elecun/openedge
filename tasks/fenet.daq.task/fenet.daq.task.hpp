
/**
 * @file    fenet.daq.task.hpp
 * @brief   Data Aquition Task for LSIS FEnet
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_FENET_DAQ_TASK_HPP_
#define _OPENEDGE_FENET_DAQ_TASK_HPP_

#include <openedge/core/task.hpp>
#include <map>

using namespace oe;

class fenetDaqTask : public core::task::runnable {
    public:
        fenetDaqTask() = default;
        virtual ~fenetDaqTask() = default;

        //common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;
};

EXPORT_RT_TASK_API

#endif