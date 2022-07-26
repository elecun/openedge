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
#include <3rdparty/mosquitto/cpp/mosquittopp.h>
#include <memory>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace oe;
using namespace std;

class device_dx3000_motor : public core::task::runnable_rt, private mosqpp::mosquittopp {
    public:
        device_dx3000_motor():mosqpp::mosquittopp(){};
        ~device_dx3000_motor() = default;

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

        /* for mqtt */
        bool _connected = false;
        string _broker_address { "127.0.0.1" };
        int _broker_port {1883};
        string _pub_topic = {"undefined"};
        int _pub_qos = 2;
        int _keep_alive = {60};
        vector<string> _mqtt_sub_topics;

        /* for device */
        double _gear_ratio = 0.0;
        int _default_rpm = 0;
        int _slave_id = 0;

        /* for UDP */
        int _gateway_port = 4002;
        string _gateway_ip = "127.0.0.1";
        long _sockfd = -1;
        long _sock_optval = 1;
        sockaddr_in _sockname;
        struct sockaddr_in targetAddr;
        

}; /* end class */

EXPORT_TASK_API

#endif