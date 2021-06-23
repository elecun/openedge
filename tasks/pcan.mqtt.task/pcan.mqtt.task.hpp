/**
 * @file    pcan.mqtt.task.hpp
 * @brief   Peak CAN Ethernet Gateway to MQTT Re-transmission
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_PCAN_MQTT_TASK_HPP_
#define _OPENEDGE_PCAN_MQTT_TASK_HPP_

#include <openedge/core.hpp>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include "pcan.parse.hpp"

namespace zmq {
    #include <czmq.h>
}

using namespace oe;
using namespace std;

class pcanMqttTask : public oe::core::task::runnable, private mosqpp::mosquittopp {
    public:
        pcanMqttTask():mosqpp::mosquittopp(){};
        virtual ~pcanMqttTask() = default;

        //common interface
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

    private:
        //PCAN data parse
        int parseDataMsg(unsigned char * p_buff, int len, S_LAN_MSG *p_msg);
        int printData(S_LAN_MSG *p_msg);

    private:
        zmq::zsock_t* _push = nullptr;
        unsigned int _data_port = 50000;

    private: //for mqtt
        string _mqtt_host {"127.0.0.1"};
        int _mqtt_port {1883};
        string _mqtt_topic = {"undefined"};
        int _mqtt_keep_alive = {60};

    private: //for pcan UDP
        long _sockfd = -1;
        long _sock_optval = 1;
        sockaddr_in _sockname;

        
        fd_set _fds_rd;
        fd_set _fds_wr;
        int _so_fd_data_in = -1;
        unsigned int _dataPort = 50000;
        int _protocol_type = SOCK_DGRAM;

};

EXPORT_TASK_API

#endif