/**
 * @file    modbusRTU.task.hpp
 * @brief   Modbus RTU service task
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_MODBUSRTU_TASK_HPP_
#define _OPENEDGE_MODBUSRTU_TASK_HPP_

#include <openedge/core.hpp>

namespace zmq {
    #include <czmq.h>
}

using namespace oe;

class modbusRTUTask : public oe::core::task::runnable {
    public:
        modbusRTUTask() = default;
        virtual ~modbusRTUTask() = default;

        //common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;
        void pause() override;
        void resume() override;

    private:
        zmq::zsock_t* _pull = nullptr;

};

EXPORT_TASK_API

#endif