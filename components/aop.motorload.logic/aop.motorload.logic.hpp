/**
 * @file aop.motorload.logic.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Current Sensor to control the AC motor
 * @version 0.1
 * @date 2022-10-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_AOP_UVLC_MOTORLOAD_LOGIC_HPP_
#define _OPENEDGE_AOP_UVLC_MOTORLOAD_LOGIC_HPP_

#include <openedge/core.hpp>
#include <memory>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>
#include <queue>

using namespace oe;
using namespace std;

class aop_motorload_logic : public core::task::runnable_rt, private mosqpp::mosquittopp {

    public:
        aop_motorload_logic():mosqpp::mosquittopp(){};
        ~aop_motorload_logic() = default;

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
        /* check function */
        bool is_over_current();

        /* function */
        void move_stop();

    private:
        double _lower_bound = 0.0;
        double _upper_bound = 0.0;
        int _mean_filter =  1;
        string _aio_name;
        deque<double> _ai_buffer;
        double _mean_value = 0.0;

    private:
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