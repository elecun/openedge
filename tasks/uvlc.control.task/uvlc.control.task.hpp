/**
 * @file    uvcontrol.task.hpp
 * @brief   UVC Lamp Cleaning System Control Task
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_UVLC_CONTROL_TASK_HPP_
#define _OPENEDGE_UVLC_CONTROL_TASK_HPP_

#include <openedge/core.hpp>

namespace zmq {
    #include <czmq.h>
}

using namespace oe;
using namespace std;

namespace oe::device {
    class controller;
    class bus;
    class gpio;
}

class uvlcControlTask : public oe::core::task::runnable {
    public:
        uvlcControlTask() = default;
        virtual ~uvlcControlTask() = default;

        //common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;
        void pause() override;
        void resume() override;

    private:
        zmq::zsock_t* _push = nullptr;
        device::controller* _controller = nullptr;
        map<int, device::gpio*> _limitsw; //60(motor side)

};

EXPORT_TASK_API

#endif