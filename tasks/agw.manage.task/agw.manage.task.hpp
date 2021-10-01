/**
 * @file    agw.manage.task.hpp
 * @brief   Application gateway management task (monitoring resource)
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_AGW_MANAGE_TASK_HPP_
#define _OPENEDGE_AGW_MANAGE_TASK_HPP_

#include <openedge/core.hpp>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>

using namespace oe;
using namespace std;

// use mqtt
class mqttConnector : public mosqpp::mosquittopp {
    public:
        mqttConnector():mosqpp::mosquittopp(){

        }
        virtual ~mqttConnector() = default;

        bool open(json param);
        void close();

        bool is_connected() const { return _connected; }
        int reconnect();

        //MQTT Callback functions
        void on_connect(int rc) override;
		void on_disconnect(int rc) override;
		void on_publish(int mid) override;
		virtual void on_message(const struct mosquitto_message* message) override;
		void on_subscribe(int mid, int qos_count, const int* granted_qos) override;
		void on_unsubscribe(int mid) override;
		void on_log(int level, const char* str) override;
		void on_error() override;

    protected:
        string _manage_topic {""};
        bool _connected = false;
        string _broker_address { "127.0.0.1" };
        int _broker_port {1883};
        string _mqtt_pub_topic = {"undefined"};
        int _mqtt_pub_qos = 2;
        int _mqtt_keep_alive = {60};
        vector<string> _mqtt_sub_topics;
};

// task class
class agwManageTask : public oe::core::task::runnable, private mosqpp::mosquittopp  {

    public:
        agwManageTask() = default;
        virtual ~agwManageTask() = default;

        //component common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;
        void pause() override;
        void resume() override;

    private:
        void on_connect(int rc) override;
		void on_disconnect(int rc) override;
		void on_publish(int mid) override;
		void on_message(const struct mosquitto_message* message);
		void on_subscribe(int mid, int qos_count, const int* granted_qos);
		void on_unsubscribe(int mid) override;
		void on_log(int level, const char* str) override;
		void on_error() override;

};

EXPORT_TASK_API

#endif