/**
 * @file test.tcp.connector.task.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Test component for TCP client with sockpp
 * @version 0.1
 * @date 2022-07-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_TEST_TCP_CONNECTOR_TASK_HPP_
#define _OPENEDGE_TEST_TCP_CONNECTOR_TASK_HPP_

#include <openedge/core.hpp>
#include <openedge/common/bus.hpp>
#include <memory>

using namespace oe;
using namespace std;

class test_tcp_connector_task : public core::task::runnable_nt {
    public:
        test_tcp_connector_task();
        ~test_tcp_connector_task();

        /* basic interface functions for nt */
        virtual void execute() override;
        virtual void stop() override;
        virtual bool configure() override;
        virtual void cleanup() override;
        virtual void pause() override;
        virtual void resume() override;

        virtual void on_request(std::any data) override;

    private:
        bus::sync_bus* _device = nullptr;   //serial device
        

}; /* end class */

EXPORT_TASK_API

#endif