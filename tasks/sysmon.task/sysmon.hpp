
/**
 * @file    sysmon.hpp
 * @brief   task for system monitoring
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SYSMON_TASK_HPP_
#define _OPENEDGE_SYSMON_TASK_HPP_

#include <openedge/core.hpp>
#include <zmq.hpp>

class sysmon : public oe::core::task::runnable {
    public:
        sysmon() = default;
        virtual ~sysmon() = default;

    //common task interface functions
    bool configure() override;
    void execute() override;
    void cleanup() override;
    void pause() override;
    void resume() override;

    private:
        //zmq::zsock_t* _tcp_push = nullptr;

    private:
        zmq::context_t* _zmq_context = nullptr;
        zmq::socket_t*  _zmq_push_socket = nullptr;
};

EXPORT_TASK_API

#endif