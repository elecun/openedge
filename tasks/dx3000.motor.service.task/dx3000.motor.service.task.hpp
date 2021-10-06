/**
 * @file    dx3000.motor.service.task.hpp
 * @brief   DX3000 AC Motor Controller Service Task with UDP
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_DX3000_MOTOR_SERVICE_TASK_HPP_
#define _OPENEDGE_DX3000_MOTOR_SERVICE_TASK_HPP_

#include <openedge/core.hpp>
#include <string>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace oe;
using namespace std;

namespace oe::device {
    class controller;
    class bus;
}

class dx3000MotorServiceTask : public oe::core::task::runnable, private mosqpp::mosquittopp  {

    enum class PUBLISH_METHOD : int { 
        ON_UPDATE,
        ON_CHANGE
    };

    public:
        dx3000MotorServiceTask():mosqpp::mosquittopp(){};
        virtual ~dx3000MotorServiceTask() = default;

        //component common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;
        void pause() override;
        void resume() override;

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

    private: //for UDP network
        int _port = 4002;
        string _gateway = "127.0.0.1";
        long _sockfd = -1;
        long _sock_optval = 1;
        sockaddr_in _sockname;
        struct sockaddr_in targetAddr;

    private:
        device::controller* _controller = nullptr;
        map<string, int> _dx_command {
            {"move_cw", 1},
            {"move_ccw", 2},
            {"stop", 3},
            {"set_rpm", 4}
        };

    private: //for mqtt
        string _manage_topic {""};
        bool _connected = false;
        string _broker_address { "127.0.0.1" };
        int _broker_port {1883};
        string _mqtt_pub_topic = {"undefined"};
        int _mqtt_pub_qos = 2;
        int _mqtt_keep_alive = {60};
        vector<string> _mqtt_sub_topics;
        PUBLISH_METHOD _pub_method = PUBLISH_METHOD::ON_UPDATE;

};

EXPORT_TASK_API

#endif