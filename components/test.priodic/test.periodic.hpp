/**
 * @file test.periodic.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Periodic Task Test
 * @version 0.1
 * @date 2022-07-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_TEST_PERIODIC_COMPONENT_HPP_
#define _OPENEDGE_TEST_PERIODIC_COMPONENT_HPP_

#include <openedge/core.hpp>
#include <memory>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>

using namespace oe;
using namespace std;

class test_periodic : public core::task::runnable_rt, private mosqpp::mosquittopp {


    public:
        test_periodic():mosqpp::mosquittopp(){};
        ~test_periodic() = default;

        /* basic interface functions for nt */
        virtual void execute() override;
        virtual void stop() override;
        virtual bool configure() override;
        virtual void cleanup() override;
        virtual void pause() override;
        virtual void resume() override;

    private:
        //MQTT Callback functions
        virtual void on_connect(int rc) override;
		virtual void on_disconnect(int rc) override;
		virtual void on_publish(int mid) override;
		virtual void on_message(const struct mosquitto_message* message) override;
		virtual void on_subscribe(int mid, int qos_count, const int* granted_qos) override;
		virtual void on_unsubscribe(int mid) override;
		virtual void on_log(int level, const char* str) override;
		virtual void on_error() override;
        

}; /* end class */

EXPORT_TASK_API

#endif