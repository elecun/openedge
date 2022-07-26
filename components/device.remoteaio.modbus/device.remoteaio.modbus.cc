
#include "device.remoteaio.modbus.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static device_remoteaio_modbus* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new device_remoteaio_modbus(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void device_remoteaio_modbus::execute(){
    if(_modbus){
        /* publish the AI Values (publish if changed) */
        unsigned short _prev_ai_values[CHANNELS*2] = {0x0000, };
        memcpy(_prev_ai_values, _ai_values, sizeof(unsigned short)*CHANNELS*2);

        //1. read AI data
        if(modbus_read_input_registers(_modbus, _ai_address, CHANNELS*2, _ai_values)!=-1){
            string val = "";
            for(int i=0; i<CHANNELS; i++){
                u.l = ((unsigned long)_ai_values[i*2+1]<<16 | _ai_values[i*2]);
                _ai_value_container[_ai_container[i]] = u.f;
                val += fmt::format("[{}]={}\t", _ai_container[i], u.f);
            }
            console::info("{}", val);
        }
        else
            console::error("Modbus read AI register error : {}", modbus_strerror(errno));

        /* publish data if changed */
        bool changed = false;
        for(int i=0;i<CHANNELS;i++){
            u.l = ((unsigned long)_prev_ai_values[i*2+1]<<16 | _prev_ai_values[i*2]);
            if(u.f!=_ai_value_container[_ai_container[i]]){
                changed = true;
                break;
            }
        }

        if(changed){
            json pubdata;
            pubdata["ai"] = _ai_value_container;
            string str_pubdata = pubdata.dump();
            this->publish(nullptr, _pub_topic.c_str(), strlen(str_pubdata.c_str()), str_pubdata.c_str(), 2, false);   
            console::info("Published@{}", _pub_topic);
        }
    }
 
}

void device_remoteaio_modbus::stop(){

}

bool device_remoteaio_modbus::configure(){
    try {

        const json& profile = this->get_profile()->raw();
        if(profile.contains(PROFILE_CONFIGURATIONS_KEY)){
            json config = profile[PROFILE_CONFIGURATIONS_KEY];

            /* device configurations */
            if(!config.contains("device")){
                console::error("device configurations does not exist");
                return false;
            }

            json device_param = config["device"];
            string _device_ip = device_param["ip"].get<string>();

            json _device_di = device_param["ai"];
            for(json::iterator itr=_device_di.begin(); itr!=_device_di.end(); ++itr){
                console::info("> Config AI : {}({})", (*itr)["name"].get<std::string>(), (*itr)["pin"].get<int>());
                _ai_container.insert(make_pair((*itr)["pin"].get<int>(), (*itr)["name"].get<std::string>()));
                _ai_value_container.insert(make_pair((*itr)["name"].get<std::string>(), false));
            }

            /* modbus configurations */
            json _device_modbus = device_param["modbus_tcp"];
            int _modbus_port = _device_modbus["port"].get<int>();
            _ai_address = _device_modbus["ai_address"].get<int>();
            console::info("> Modbus TCP Port : {}", _modbus_port);
            console::info("> Modbus AI Address : {}", _ai_address);

            if(!_modbus){
                _modbus = modbus_new_tcp(_device_ip.c_str(), _modbus_port);
                if(!_modbus){
                    console::error("Unable to create the modbus context");
                    return false;
                }

                modbus_set_response_timeout(_modbus, 1, 0); //response time 1s

                if(modbus_connect(_modbus)==-1){
                    console::error("Device connection failed : {}", modbus_strerror(errno));
                    modbus_free(_modbus);
                    return false;
                }
                else
                    console::info("Successfully connected to device {}", _device_ip);
            }

            /* mqtt configurations */
            if(!config.contains("mqtt")){
                console::error("MQTT configurations does not exist");
                return false;
            }

            json mqtt_param = config["mqtt"];
            _broker_address = mqtt_param["broker"].get<string>();
            _broker_port = mqtt_param["port"].get<int>();
            _pub_topic = mqtt_param["pub_topic"].get<string>();
            _pub_qos = mqtt_param["pub_qos"].get<int>();
            _keep_alive = mqtt_param["keep_alive"].get<int>();
            if(mqtt_param.find("sub_topic")!=mqtt_param.end()){
                for(json::iterator itr=mqtt_param["sub_topic"].begin(); itr!=mqtt_param["sub_topic"].end(); ++itr){
                    _sub_topics.emplace_back(*itr);
                }
            }

            console::info("> set MQTT Broker : {}", _broker_address);
            console::info("> set MQTT Port : {}", _broker_port);
            console::info("> set MQTT Pub. Topic : {}", _pub_topic);
            console::info("> set MQTT Pub. QoS : {}", _pub_qos);
            console::info("> set MQTT Keep-alive : {}", _keep_alive);

            //connect to MQTT broker
            if(const int conret = this->connect_async(_broker_address.c_str(), _broker_port, _keep_alive)==MOSQ_ERR_SUCCESS){
                for(string topic:_sub_topics){
                    this->subscribe(nullptr, topic.c_str(), 2);
                    console::info("> Subscribe topic : {}", topic);
                }
                    
                if(const int ret = this->loop_start()!=MOSQ_ERR_SUCCESS)
                    console::warn("({}){}", ret, mosqpp::strerror(ret));
            }
            else
                console::warn("({}){}", conret, mosqpp::strerror(conret));
        }
        else
            return false;

    }
    catch(const json::exception& e){
        console::error("Error : {}", e.what());
        return false;
    }

    return true;
}

void device_remoteaio_modbus::cleanup(){
    /* modbus connection close */
    if(_modbus){
        modbus_close(_modbus);
        modbus_free(_modbus);
        console::info("Modbus connection closed");
    }

    /* mqtt connection close */
    this->mosqpp::mosquittopp::disconnect();
    this->mosqpp::mosquittopp::loop_stop();
    mosqpp::lib_cleanup();

}

void device_remoteaio_modbus::pause(){
    
}

void device_remoteaio_modbus::resume(){
    
}

void device_remoteaio_modbus::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Broker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);

}
void device_remoteaio_modbus::on_disconnect(int rc){

}
void device_remoteaio_modbus::on_publish(int mid){

}
void device_remoteaio_modbus::on_message(const struct mosquitto_message* message){

}
void device_remoteaio_modbus::on_subscribe(int mid, int qos_count, const int* granted_qos){

}
void device_remoteaio_modbus::on_unsubscribe(int mid){

}
void device_remoteaio_modbus::on_log(int level, const char* str){

}
void device_remoteaio_modbus::on_error(){
    
}

