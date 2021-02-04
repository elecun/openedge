/**
 * @file    uvcontrol.task.hpp
 * @brief   UVC Lamp Cleaning System Control Task
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_UVCONTROL_TASK_HPP_
#define _OPENEDGE_UVCONTROL_TASK_HPP_

#include <openedge/core.hpp>
namespace zmq {
    #include <czmq.h>
}

using namespace oe;

class uvcontrolTask : public oe::core::task::runnable {
    public:
        uvcontrolTask() = default;
        virtual ~uvcontrolTask() = default;

        //common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;
        void pause() override;
        void resume() override;

    private:
        zmq::zsock_t* _push = nullptr;

};

EXPORT_TASK_API

#endif