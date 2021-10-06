/**
 * @file    moxa.io.service.task.hpp
 * @brief   MOXA Nport Serial Device Server
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_MOXA_IO_TASK_HPP_
#define _OPENEDGE_MOXA_IO_TASK_HPP_

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

class moxaIoServiceTask : public oe::core::task::runnable, protected mosqpp::mosquittopp {

    enum class PUBLISH_METHOD : int { 
        ON_UPDATE,
        ON_DI_CHANGE
    };

    public:
        moxaIoServiceTask():mosqpp::mosquittopp(){};
        virtual ~moxaIoServiceTask() = default;

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
        /* internal service function */
        void set_DO(unsigned short value);
        void set_DO_or(unsigned short value); //apply logical(OR)

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
        int _di_address = -1;
        int _do_address = -1;
        
        map<int, string> _di_container;
        map<string, bool> _di_value_container;
        map<int, string> _do_container;
        map<string, bool> _do_value_container;
        unsigned short _di_values;
        unsigned short _do_values;

    private: //for mqtt service command
        map<string, int> _service_cmd {
            {"set_on", 1}, // with name defined in profile
            {"set_off", 2} // with name defined in profile
        };

};

EXPORT_TASK_API

#endif