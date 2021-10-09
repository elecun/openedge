/**
 * @file    mex.loadcell.service.task.hpp
 * @brief   loadcell data reader service for mex
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_MEX_LOADCELL_SERVICE_TASK_HPP_
#define _OPENEDGE_MEX_LOADCELL_SERVICE_TASK_HPP_

#include <openedge/core.hpp>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>
#include <string>
#include <vector>
#include <map>
#include <exception>
#include <3rdparty/CppLinuxSerial/SerialPort.hpp>

using namespace oe;
using namespace std;
using namespace mn::CppLinuxSerial;

class mexLoadcellServiceTask : public oe::core::task::runnable, private mosqpp::mosquittopp {

    enum class PUBLISH_METHOD : int { 
        ON_UPDATE,
        ON_CHANGE
    };

    public:
        mexLoadcellServiceTask():mosqpp::mosquittopp(){};
        virtual ~mexLoadcellServiceTask() = default;

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

    private: //for loadcell
        string _port = "/dev/ttyS0";
        unsigned int _baudrate = 9600;
        SerialPort* _tty = nullptr;


};

EXPORT_TASK_API

#endif