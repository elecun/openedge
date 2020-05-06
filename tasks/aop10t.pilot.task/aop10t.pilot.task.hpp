
/**
 * @file    aop10t.pilot.task.hpp
 * @brief   AOP 10t-scale Pilot Data Collection Task
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_AOP10T_PILOT_TASK_HPP_
#define _OPENEDGE_AOP10T_PILOT_TASK_HPP_

#include <openedge/core/task.hpp>
#include <openedge/core/service.hpp>
#include <map>
#include <string>
#include <memory>

using namespace oe;
using namespace std;

class aop10tPilotTask : public core::task::runnable {
    public:
        aop10tPilotTask() = default;
        virtual ~aop10tPilotTask() = default;

        //common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;

    private:
        

};

EXPORT_RT_TASK_API

#endif