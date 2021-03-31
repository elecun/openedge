
/**
 * @file    sysmon.hpp
 * @brief   task for system monitoring, publish the status data
 *          system status & task information will be published periodically.
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SYSMON_TASK_HPP_
#define _OPENEDGE_SYSMON_TASK_HPP_

#include <openedge/core.hpp>
#include <openedge/util.hpp>
#include <zmq.hpp>
#include <sys/sysinfo.h>
#include <sys/types.h>

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
        int current_vmem_used();

    private:
        zmq::context_t* _zmq_context = nullptr;
        zmq::socket_t*  _zmq_socket_pub = nullptr;

        struct sysinfo _sysinfo;

        oe::sys::cpuload _cpuinfo;
        oe::sys::netload _netinfo;
};

EXPORT_TASK_API

#endif