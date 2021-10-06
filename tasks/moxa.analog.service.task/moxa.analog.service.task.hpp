/**
 * @file    moxa.analog.service.task.hpp
 * @brief   MOXA Analog I/O service task
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_MOXA_ANALOG_SERVICE_TASK_HPP_
#define _OPENEDGE_MOXA_ANALOG_SERVICE_TASK_HPP_

#include <openedge/core.hpp>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <map>
#include <string>
#include <openedge/net/udpserver.h>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>
#include <3rdparty/libmodbus/modbus.h>

using namespace oe;
using namespace std;

class moxaAnalogServiceTask : public oe::core::task::runnable, protected mosqpp::mosquittopp {

    enum class PUBLISH_METHOD : int { 
        ON_UPDATE,
        ON_AI_CHANGE
    };

    union typeunion {
        unsigned long l;
        float f;
    } u;

    #define CHANNELS    8

    public:
        moxaAnalogServiceTask():mosqpp::mosquittopp(){};
        virtual ~moxaAnalogServiceTask() = default;

        /* common component interface */
        bool configure() override;
        void execute() override;
        void cleanup() override;
        void pause() override;
        void resume() override;

    private:
        virtual void on_connect(int rc) override;
		virtual void on_disconnect(int rc) override;
		virtual void on_publish(int mid) override;
		virtual void on_message(const struct mosquitto_message* message) override;
		virtual void on_subscribe(int mid, int qos_count, const int* granted_qos) override;
		virtual void on_unsubscribe(int mid) override;
		virtual void on_log(int level, const char* str) override;
		virtual void on_error() override;

    private:

        bool read_device_config(json& config);
        bool read_mqtt_config(json& config);

    private:   //services
        // void service_set_on(json& msg);  //set_on mqtt command
        // void service_set_off(json& msg); //set_off mqtt command

    private: //for modbus
        modbus_t* _modbus = nullptr;
        int _modbus_port = 502;

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


    private: //for io
        string _devicename = "unknown";
        string _deviceip = "127.0.0.1";
        int _ai_address = -1;
        
        map<int, string> _ai_container;
        map<string, float> _ai_value_container;
        unsigned short _ai_values[CHANNELS*2];

};

EXPORT_TASK_API

#endif