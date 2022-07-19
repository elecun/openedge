/**
 * @file device.dx3000.motor.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief DKM DX3000 Motor Speed Controller
 * @version 0.1
 * @date 2022-07-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_DEVICE_DKM_DX3000_MOTOR_HPP_
#define _OPENEDGE_DEVICE_DKM_DX3000_MOTOR_HPP_

#include <openedge/core.hpp>
#include <openedge/common/bus.hpp>
#include <openedge/device/controller.hpp>
#include <memory>

using namespace oe;
using namespace std;

class device_dx3000_motor : public core::task::runnable_rt, private mosqpp::mosquittopp {
    public:
        device_dx3000_motor();
        ~device_dx3000_motor();

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

        device::controller* _controller = nullptr;
        map<string, int> _dx_command {
            {"move_cw", 1},     //move clock-wise direction
            {"move_ccw", 2},    //move counter clock-wise direction
            {"stop", 3},        //stop
            {"set_rpm", 4}      //set speed
        };

        string _mqtt_broker {"127.0.0.1"};
        int _mqtt_port {1883};
        string _mqtt_pub_topic = {"undefined"};
        int _mqtt_pub_qos = 2;
        int _mqtt_keep_alive = {60};
        vector<string> _mqtt_sub_topics;
        

}; /* end class */

EXPORT_TASK_API

#endif