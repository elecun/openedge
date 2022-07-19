/**
 * @file aop.uvlc.logic.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief UV Lamp Cleaning System Control Logic on Openedge Framework
 * @version 0.1
 * @date 2022-07-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_AOP_UVLC_LOGIC_HPP_
#define _OPENEDGE_AOP_UVLC_LOGIC_HPP_

#include <openedge/core.hpp>
#include <memory>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>

using namespace oe;
using namespace std;

class aop_uvlc_logic : public core::task::runnable_rt, private mosqpp::mosquittopp {

    enum class PUBLISH_METHOD : int { 
        ON_UPDATE,
        ON_CHANGE
    };

    /* UVLC control mode */
    enum class CONTROLMODE : int { 
        MANUAL = 0,
        AUTOMATIC = 1
    };

    /* UVLC limit active check by Proximity sensors */
    enum class LIMIT_ACTIVE : int { 
        NO_LIMIT_ACTIVE = 0,
        FORWARD_LIMIT_ACTIVE = 1,
        REVERSE_LIMIT_ACTIVE = 2,
        BOTH_LIMIT_ACTIVE = 3
    };

    /* system working state */
    enum class UVLC_WORK_STATE : int {
        READY = 0,
        WORK = 1
    };

    public:
        aop_uvlc_logic();
        ~aop_uvlc_logic();

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
        string _manage_topic {""};
        bool _connected = false;
        string _broker_address { "127.0.0.1" };
        int _broker_port {1883};
        string _mqtt_pub_topic = {"undefined"};
        int _mqtt_pub_qos = 2;
        int _mqtt_keep_alive = {60};
        vector<string> _mqtt_sub_topics;
        PUBLISH_METHOD _pub_method = PUBLISH_METHOD::ON_UPDATE;

        string _l_proximity_i, _l_proximity_o;
        string _r_proximity_i, _r_proximity_o;
        string _wipe_forward_i, _wipe_forward_o;
        string _wipe_reverse_i, _wipe_reverse_o;
        string _wipe_once_i, _wipe_once_o;
        string _stop_i, _stop_o;
        
        bool _l_proximity_value = false;
        bool _r_proximity_value = false;
        bool _wipe_forward_value = false;
        bool _wipe_reverse_value = false;
        bool _wipe_once_value = false;
        bool _stop_value = false;
        

}; /* end class */

EXPORT_TASK_API

#endif