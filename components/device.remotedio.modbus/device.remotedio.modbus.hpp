/**
 * @file device.remotedio.modbus.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Remote DIO Controller Device component (for MOXA IoLogik E1212)
 * @version 0.1
 * @date 2022-07-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_REMOTE_DIO_MODBUS_COMPONENT_HPP_
#define _OPENEDGE_REMOTE_DIO_MODBUS_COMPONENT_HPP_

#include <openedge/core.hpp>
#include <memory>
#include <map>
#include <string>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>
#include <3rdparty/libmodbus/modbus.h>

using namespace oe;
using namespace std;

class device_remotedio_modbus : public core::task::runnable_rt, private mosqpp::mosquittopp {


    public:
        device_remotedio_modbus():mosqpp::mosquittopp(){};
        ~device_remotedio_modbus() = default;

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
        /* for modbus */
        modbus_t* _modbus = nullptr;
        int _modbus_port = 502;

        /* for io map */
        map<int, string> _di_container;
        map<string, bool> _di_value_container;
        map<int, string> _do_container;
        map<string, bool> _do_value_container;
        unsigned short _di_values;
        unsigned short _do_values;
        int _di_address = -1;
        int _do_address = -1;

        /* for mqtt */
        bool _connected = false;
        string _broker_address { "127.0.0.1" };
        int _broker_port {1883};
        string _pub_topic = {"undefined"};
        int _pub_qos = 2;
        int _keep_alive = {60};
        vector<string> _sub_topics;
        

}; /* end class */

EXPORT_TASK_API

#endif