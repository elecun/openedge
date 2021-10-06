/**
 * @file    uvlc.control.logic.task.hpp
 * @brief   UVC Lamp Cleaning System Control Task (IO Interface & Control Logic)
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_UVLC_CONTROL_TASK_HPP_
#define _OPENEDGE_UVLC_CONTROL_TASK_HPP_

#include <openedge/core.hpp>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>
#include <string>
#include <vector>
#include <map>

using namespace oe;
using namespace std;

class uvlcControLogiclTask : public oe::core::task::runnable, private mosqpp::mosquittopp {

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
        uvlcControLogiclTask():mosqpp::mosquittopp(){};
        virtual ~uvlcControLogiclTask() = default;

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

        void setmode(CONTROLMODE mode);

    private: //for logic
        bool check_stop_enable(const bool value);
        bool check_l_proximity_enable(const bool value);
        bool check_r_proximity_enable(const bool value);
        bool check_wipe_forward(const bool value);
        bool check_wipe_reverse(const bool value);
        bool check_wipe_once(const bool value);


    private:
        CONTROLMODE _control_mode {CONTROLMODE::MANUAL };

        unsigned short _limit_id = 0x600;
        map<string, unsigned short> _intensity_id;
        unsigned char _limit_value = 0xc0;    //limit sensor value
        map<unsigned short, float> _intensity_value;
        float _intensity_threshold = 0.0;

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

    private: //for logic
        string _l_proximity_io;
        string _r_proximity_io;
        string _wipe_forward_io;
        string _wipe_reverse_io;
        string _wipe_once_io;
        string _stop_io;
        bool _l_proximityu_io_value = false;
        bool _r_proximityu_io_value = false;
        bool _wipe_forward_io_value = false;
        bool _wipe_reverse_io_value = false;
        bool _wipe_once_io_value = false;
        bool _stop_io_value = false;


};

EXPORT_TASK_API

#endif