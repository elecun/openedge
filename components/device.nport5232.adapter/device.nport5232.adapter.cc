
#include "device.nport5232.adapter.hpp"
#include <openedge/log.hpp>
#include <3rdparty/mosquitto/cpp/mosquittopp.h>
#include <support/device/dkm/dx3000.hpp>

//static component instance that has only single instance
static device_nport5232_adapter* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new device_nport5232_adapter(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

device_nport5232_adapter::device_nport5232_adapter(){


}

device_nport5232_adapter::~device_nport5232_adapter(){

}

void device_nport5232_adapter::execute(){

}

void device_nport5232_adapter::stop(){

}

bool device_nport5232_adapter::configure(){

    try {
        /* close controller*/
        if(_controller){
            _controller->close();
            delete _controller;
        }

        /* mosquitto initialization */
        if(const int ret = mosqpp::lib_init()!=MOSQ_ERR_SUCCESS){
            console::error("({}){}", ret, mosqpp::strerror(ret));
            return false;
        }

        /* read configuration of its profile */
        const json& prof = this->get_profile()->raw();

        /* configuration for motor driver */
        if(prof.contains(PROFILE_CONFIGURATIONS_KEY)){
            json conf = prof[PROFILE_CONFIGURATIONS_KEY];

            if(conf.contains("dx3000")){
                json dx3000_param = conf["dx3000"];
                int rpm = dx3000_param["rpm"].get<int>();
                int slave_id = dx3000_param["slave_id"].get<int>();
                string access = dx3000_param["access"].get<string>();

                _controller = new support::DKM_DX3000(slave_id, access.c_str());
                if(!_controller->open()){
                    console::error("Cannot open the DKM DX3000 Motor Driver");
                    return false;
                }

                support::DKM_DX3000* motor = dynamic_cast<support::DKM_DX3000*>(_controller);
                if(motor){
                    motor->set_parameter(support::DKM_DX3000::PARAMETER::SET_ID, slave_id);
                    motor->set_rpm(rpm);
                    console::info("Use DX3000 Controller ({}, {})", access, slave_id);
                }
            }
            else {
                console::error("cannot read configurations of DX3000 in profile");
                return false;
            }

            /* MQTT Configurations */
        if(prof.contains("mqtt")){

            json mqtt_param = conf["mqtt"];
            if(mqtt_param.find("broker")!=mqtt_param.end()) _mqtt_broker = mqtt_param["broker"].get<string>();
            if(mqtt_param.find("port")!=mqtt_param.end()) _mqtt_port = mqtt_param["port"].get<int>();
            if(mqtt_param.find("pub_topic")!=mqtt_param.end()) _mqtt_pub_topic = mqtt_param["pub_topic"].get<string>();
            if(mqtt_param.find("pub_qos")!=mqtt_param.end()) _mqtt_pub_qos = mqtt_param["pub_qos"].get<int>();
            if(mqtt_param.find("keep_alive")!=mqtt_param.end()) _mqtt_keep_alive = mqtt_param["keep_alive"].get<int>();
            if(mqtt_param.find("sub_topic")!=mqtt_param.end()){
                for(json::iterator itr=mqtt_param["sub_topic"].begin(); itr!=mqtt_param["sub_topic"].end(); ++itr){
                    _mqtt_sub_topics.emplace_back(*itr);
                }
            } 

            console::info("> set MQTT Broker : {}", _mqtt_broker);
            console::info("> set MQTT Port : {}", _mqtt_port);
            console::info("> set MQTT Pub. Topic : {}", _mqtt_pub_topic);
            console::info("> set MQTT Pub. QoS : {}", _mqtt_pub_qos);
            console::info("> set MQTT Keep-alive : {}", _mqtt_keep_alive);

            //connect to MQTT broker
            if(const int conret = this->connect_async(_mqtt_broker.c_str(), _mqtt_port, _mqtt_keep_alive)==MOSQ_ERR_SUCCESS){
                for(string topic:_mqtt_sub_topics){
                    this->subscribe(nullptr, topic.c_str(), 2);
                    console::info("> set MQTT Sub. Topic : {}", topic);
                }
                    
                if(const int ret = this->loop_start()!=MOSQ_ERR_SUCCESS)
                    console::warn("({}){}", ret, mosqpp::strerror(ret));
            }
            else
                console::warn("({}){}", conret, mosqpp::strerror(conret));

            }
        }
    }
    catch(const json::exception& e){
        console::warn("cofiguration failed : {}", e.what());
    }

    return true;
    
}

void device_nport5232_adapter::cleanup(){

}

void device_nport5232_adapter::pause(){
    
}

void device_nport5232_adapter::resume(){
    
}

void device_nport5232_adapter::on_request(std::any data){

}
