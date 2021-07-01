/**
 * @file    agsys.manage.task.hpp
 * @brief   Application Gateway Management Task
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_AGSYS_MANAGE_TASK_HPP_
#define _OPENEDGE_AGSYS_MANAGE_TASK_HPP_

#include <openedge/core.hpp>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>
#include <string>

namespace zmq {
    #include <czmq.h>
}

using namespace oe;
using namespace std;


class agsysManageTask : public oe::core::task::runnable, private mosqpp::mosquittopp  {

    public:
        agsysManageTask() = default;
        virtual ~agsysManageTask() = default;

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
        zmq::zsock_t* _push = nullptr;

        string _mqtt_broker {"127.0.0.1"};
        int _mqtt_port {1883};
        string _mqtt_pub_topic = {"undefined"};
        int _mqtt_pub_qos = 2;
        int _mqtt_keep_alive = {60};
        vector<string> _mqtt_sub_topics;

};

EXPORT_TASK_API

#endif