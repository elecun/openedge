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
#include <thread>
#include <vector>

namespace zmq {
    #include <czmq.h>
}

using namespace oe;
using namespace std;

namespace oe::device {
    class controller;
    class bus;
}

class pcanMqttTask : public oe::core::task::runnable, private mosqpp::mosquittopp {

    enum class CONTROLMODE : int { 
        MANUAL = 0,
        AUTOMATIC = 1
    };

    public:
        pcanMqttTask():mosqpp::mosquittopp(){};
        virtual ~pcanMqttTask() = default;

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

    private:
        //PCAN data parse
        int parse_pcan_data(unsigned char * p_buff, int len, S_LAN_MSG *p_msg);
        int print_pcan_data(S_LAN_MSG* p_msg);
        void setmode(CONTROLMODE mode);
        int handle_loc_cmd(int fd);
        int handle_rem_data(int fd);

    private:
        //UDP subtasking
        std::thread* _psubTask = nullptr;
        void subtask();

    private:
        zmq::zsock_t* _push = nullptr;
        device::controller* _controller = nullptr;
        CONTROLMODE _control_mode {CONTROLMODE::MANUAL };

    private: //for mqtt
        string _mqtt_broker {"127.0.0.1"};
        int _mqtt_port {1883};
        string _mqtt_pub_topic = {"undefined"};
        int _mqtt_pub_qos = 2;
        int _mqtt_keep_alive = {60};
        vector<string> _mqtt_sub_topics;

    private: //for pcan UDP
        int _pcan_dataport {50000};
        long _sockfd = -1;
        long _sock_optval = 1;
        sockaddr_in _sockname;
        fd_set _fds_rd;
        fd_set _fds_wr;

};

EXPORT_TASK_API

#endif