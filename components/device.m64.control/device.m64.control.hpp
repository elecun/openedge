/**
 * @file device.m64.control.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief M64 Acoustic Modem, underwater communication component
 * @version 0.1
 * @date 2022-09-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_DEVICE_M64_CONTROL_HPP_
#define _OPENEDGE_DEVICE_M64_CONTROL_HPP_

#include <openedge/core.hpp>
#include <memory>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>
#include <include/support/device/waterlinked/m64.hpp>

using namespace oe;
using namespace std;

class device_m64_control : public core::task::runnable_rt, private mosqpp::mosquittopp {

    public:
        device_m64_control():mosqpp::mosquittopp(){};
        ~device_m64_control() = default;

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

    private:
        string _port = "/dev/ttyS0";
        int _baudrate = 9600;
        double _timeout_s = 1.0;
        

    private:
        bool _connected = false;
        string _broker_address { "127.0.0.1" };
        int _broker_port {1883};
        string _pub_topic = {"undefined"};
        int _pub_qos = 2;
        int _keep_alive = {60};
        vector<string> _sub_topics;
        

}; /* end class */

EXPORT_TASK_API

#endif