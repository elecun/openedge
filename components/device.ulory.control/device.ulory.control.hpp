/**
 * @file device.ulory.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief SystemBase Lora Converter uLory Product control component
 * @version 0.1
 * @date 2022-09-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_DEVICE_ULORY_CONTROL_HPP_
#define _OPENEDGE_DEVICE_ULORY_CONTROL_HPP_

#include <openedge/core.hpp>
#include <memory>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>
#include <deque>
#include "ulory.hpp"

using namespace oe;
using namespace std;

class device_ulory_control : public core::task::runnable_rt, private mosqpp::mosquittopp {

    public:
        device_ulory_control():mosqpp::mosquittopp(){};
        ~device_ulory_control() = default;

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
        device::systembase::ulory* _device = nullptr;
        string _port = "/dev/ttyUSB0";
        int _baudrate = 9600;
        int _timeout_s = 1;
        int _source_id = 0;
        int _target_id = 0;

    private:
        deque<char> _dq;

    private:
        bool _mqtt_connected = false;
        string _broker_address { "127.0.0.1" };
        int _broker_port {1883};
        string _pub_topic = {"undefined"};
        int _pub_qos = 2;
        int _keep_alive = {60};
        vector<string> _sub_topics;
        

}; /* end class */

EXPORT_TASK_API

#endif