
#include "aop.uvlc.logic.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static aop_uvlc_logic* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new aop_uvlc_logic(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


aop_uvlc_logic::aop_uvlc_logic():mosqpp::mosquittopp(){
}

aop_uvlc_logic::~aop_uvlc_logic(){
}

void aop_uvlc_logic::execute(){
 
}

void aop_uvlc_logic::stop(){

}

bool aop_uvlc_logic::configure(){
    try {
        const json& profile = this->get_profile()->raw();

        /* MQTT configurations */
        if(profile.contains("mqtt")){
            json mqtt_param = profile["mqtt"];

            _broker_address = mqtt_param["broker"].get<string>();
            _broker_port = mqtt_param["port"].get<int>();
            _mqtt_pub_topic = mqtt_param["pub_topic"].get<string>();
            _mqtt_pub_qos = mqtt_param["pub_qos"].get<int>();
            _mqtt_keep_alive = mqtt_param["keep_alive"].get<int>();
            for(json::iterator itr=mqtt_param["sub_topic"].begin(); itr!=mqtt_param["sub_topic"].end(); ++itr){
                _mqtt_sub_topics.emplace_back(*itr);
            }

            console::info("> set MQTT Broker : {}", _broker_address);
            console::info("> set MQTT Port : {}", _broker_port);
            console::info("> set MQTT Pub. Topic : {}", _mqtt_pub_topic);
            console::info("> set MQTT Pub. QoS : {}", _mqtt_pub_qos);
            console::info("> set MQTT Keep-alive : {}", _mqtt_keep_alive);

            //connect to MQTT broker
            if(const int conret = this->connect_async(_broker_address.c_str(), _broker_port, _mqtt_keep_alive)==MOSQ_ERR_SUCCESS){
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

        /* IO Interface */
        if(profile.contains("io")){
            json io_param = profile["io"];
            _l_proximity = io_param["l_proximity_i"].get<string>();
            _r_proximity = io_param["r_proximity_i"].get<string>();
            _wipe_once = io_param["wipe_once_i"].get<string>();
            _wipe_forward = io_param["wipe_forward_i"].get<string>();
            _wipe_reverse = io_param["wipe_reverse_i"].get<string>();
            _stop = io_param["stop_i"].get<string>();

        }
        if(config.find("parameter")!=config.end()){
            json uvlc_param = config["parameter"];

            if(uvlc_param.find("l_proximity_io")!=uvlc_param.end()){
                _l_proximity_io = uvlc_param["l_proximity_io"].get<string>();
                console::info("> UVLC Proximity IO(Left) : {}", _l_proximity_io);
            }
            if(uvlc_param.find("r_proximity_io")!=uvlc_param.end()){
                _r_proximity_io = uvlc_param["r_proximity_io"].get<string>();
                console::info("> UVLC Proximity IO(Right) : {}", _r_proximity_io);
            }
            if(uvlc_param.find("wipe_forward_io")!=uvlc_param.end()){
                _wipe_forward_io = uvlc_param["wipe_forward_io"].get<string>();
                console::info("> UVLC WIPE FORWARD I/O : {}", _wipe_forward_io);
            }
            if(uvlc_param.find("wipe_reverse_io")!=uvlc_param.end()){
                _wipe_reverse_io = uvlc_param["wipe_reverse_io"].get<string>();
                console::info("> UVLC WIPE REVERSE I/O : {}", _wipe_reverse_io);
            }
            if(uvlc_param.find("wipe_once_io")!=uvlc_param.end()){
                _wipe_once_io = uvlc_param["wipe_once_io"].get<string>();
                console::info("> UVLC WIPE ONCE I/O : {}", _wipe_once_io);
            }
            if(uvlc_param.find("stop_io")!=uvlc_param.end()){
                _stop_io = uvlc_param["stop_io"].get<string>();
                console::info("> UVLC WIPE STOP I/O : {}", _stop_io);
            }
            
        }



    }
    catch(const json::exception& e){
        console::error("Profile read/access error : {}", e.what());
    }

    return true;
}

void aop_uvlc_logic::cleanup(){

}

void aop_uvlc_logic::pause(){
    
}

void aop_uvlc_logic::resume(){
    
}

