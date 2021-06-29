/**
 * @file    dx3000.control.task.hpp
 * @brief   DX3000 AC Motor Controller Task
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_DX3000_CONTROL_TASK_HPP_
#define _OPENEDGE_DX3000_CONTROL_TASK_HPP_

#include <openedge/core.hpp>
#include <string>

namespace zmq {
    #include <czmq.h>
}

using namespace oe;
using namespace std;

namespace oe::device {
    class controller;
    class bus;
}

class dx3000ControlTask : public oe::core::task::runnable  {

    public:
        dx3000ControlTask() = default;
        virtual ~dx3000ControlTask() = default;

        //component common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;
        void pause() override;
        void resume() override;

};

EXPORT_TASK_API

#endif