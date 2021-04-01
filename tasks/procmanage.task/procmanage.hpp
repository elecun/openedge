
/**
 * @file    procmanage.hpp
 * @brief   openedge process manager task
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_PROCESS_MANAGE_TASK_HPP_
#define _OPENEDGE_PROCESS_MANAGE_TASK_HPP_

#include <openedge/core.hpp>
#include <openedge/util.hpp>
#include <zmq.hpp>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <deque>

using namespace std;

class procmanage : public oe::core::task::runnable {
    public:

        procmanage() = default;
        virtual ~procmanage() = default;

        /**
         * @brief   common task interface functions
         */
        bool configure() override;
        void execute() override;
        void cleanup() override;
        void pause() override;
        void resume() override;

    private:
        zmq::context_t* _zmq_context = nullptr;
        zmq::socket_t*  _zmq_dataport = nullptr;
        zmq::socket_t*  _zmq_serviceport = nullptr;

        deque<int> _sigrt_container; /* SIGRT Container */
};

EXPORT_TASK_API

#endif