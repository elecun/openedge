/**
 * @file    fenet.mqtt.task.hpp
 * @brief   LSIS PLC Fast Ethernet to MQTT
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_FENET_MQTT_TASK_HPP_
#define _OPENEDGE_FENET_MQTT_TASK_HPP_

#include <openedge/core.hpp>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>
#include <memory>
#include <fenetservice.hpp>

using namespace oe;
using namespace std;

class fenetMqttTask : public oe::core::task::runnable, private mosqpp::mosquittopp {

    public:
        fenetMqttTask():mosqpp::mosquittopp(){};
        virtual ~fenetMqttTask() = default;

        //component common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;
        void pause() override;
        void resume() override;

    private:
        //MQTT Callback functions
        void on_connect(int rc) override;
		void on_disconnect(int rc) override;
		void on_publish(int mid) override;
		void on_message(const struct mosquitto_message* message) override;
		void on_subscribe(int mid, int qos_count, const int* granted_qos) override;
		void on_unsubscribe(int mid) override;
		void on_log(int level, const char* str) override;
		void on_error() override;

    private:
        unique_ptr<oe::core::service> _service; //Fast Ethernet Service
        

    private: //for mqtt
        string _mqtt_broker {"127.0.0.1"};
        int _mqtt_port {1883};
        string _mqtt_pub_topic {"undefined"};
        int _mqtt_pub_qos {2};
        int _mqtt_keep_alive = {60};
        vector<string> _mqtt_sub_topics;

    private:



};

EXPORT_TASK_API

#endif