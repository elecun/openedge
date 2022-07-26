
#include "device.remotedio.modbus.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static device_remotedio_modbus* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new device_remotedio_modbus(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void device_remotedio_modbus::execute(){
    if(_modbus){
        /* publish the DI & DO Values (publish if changed) */
        unsigned short _prev_di_values = _di_values;

        //1. read DI data
        if(modbus_read_input_registers(_modbus, _di_address, 1, &_di_values)!=-1){
            string val = "";
            for(auto& d:_di_container){
                _di_value_container[d.second] = static_cast<bool>(_di_values&(0x0001<<d.first));
                val += fmt::format("[{}]={}\t", d.second, _di_value_container[d.second]?1:0);
            }
            console::info("{}", val);
        }
        else
            console::error("Modbus read DI register error : {}", modbus_strerror(errno));

        //2. read DO data
        if(modbus_read_registers(_modbus, _do_address, 1, &_do_values)!=-1){
            string val = "";
            for(auto& d:_do_container){
                _do_value_container[d.second] = static_cast<bool>(_do_values&(0x0001<<d.first));
                val += fmt::format("[{}]={}\t", d.second, _di_value_container[d.second]?1:0);
            }
            console::info("{}", val);
        }
        else
            console::error("Modbus read DO register error : {}", modbus_strerror(errno));


        /* publish all data */
        if(_prev_di_values!=_di_values){
            json pubdata;
            pubdata["di"] = _di_value_container;
            pubdata["do"] = _do_value_container;
            string str_pubdata = pubdata.dump();
            this->publish(nullptr, _pub_topic.c_str(), strlen(str_pubdata.c_str()), str_pubdata.c_str(), 2, false);   
            console::info("Published@{}", _pub_topic);
        }
    }
 
}

void device_remotedio_modbus::stop(){

}

bool device_remotedio_modbus::configure(){
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

            json _device_di = device_param["di"];
            for(json::iterator itr=_device_di.begin(); itr!=_device_di.end(); ++itr){
                console::info("> Config DI : {}({})", (*itr)["name"].get<std::string>(), (*itr)["pin"].get<int>());
                _di_container.insert(make_pair((*itr)["pin"].get<int>(), (*itr)["name"].get<std::string>()));
                _di_value_container.insert(make_pair((*itr)["name"].get<std::string>(), false));
            }

            json _device_do = device_param["do"];
            for(json::iterator itr=_device_do.begin(); itr!=_device_do.end(); ++itr){
                console::info("> Config DO : {}({})", (*itr)["name"].get<std::string>(), (*itr)["pin"].get<int>());
                _do_container.insert(make_pair((*itr)["pin"].get<int>(), (*itr)["name"].get<std::string>()));
                _do_value_container.insert(make_pair((*itr)["name"].get<std::string>(), false));
            }

            /* modbus configurations */
            json _device_modbus = device_param["modbus_tcp"];
            int _modbus_port = _device_modbus["port"].get<int>();
            _di_address = _device_modbus["di_address"].get<int>();
            _do_address = _device_modbus["do_address"].get<int>();
            console::info("> Modbus TCP Port : {}", _modbus_port);
            console::info("> Modbus DI Address : {}", _di_address);
            console::info("> Modbus DO Address : {}", _do_address);
            if(!_modbus){
                _modbus = modbus_new_tcp(_device_ip.c_str(), _modbus_port);
                if(!_modbus){
                    console::error("Unable to create the modbus context");
                    return false;
                }

                modbus_set_response_timeout(_modbus, 1, 0); //response time 1s
                modbus_error_recovery_mode()

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

void device_remotedio_modbus::cleanup(){
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

void device_remotedio_modbus::pause(){
    
}

void device_remotedio_modbus::resume(){
    
}

void device_remotedio_modbus::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Broker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);

}
void device_remotedio_modbus::on_disconnect(int rc){

}
void device_remotedio_modbus::on_publish(int mid){

}
void device_remotedio_modbus::on_message(const struct mosquitto_message* message){

}
void device_remotedio_modbus::on_subscribe(int mid, int qos_count, const int* granted_qos){

}
void device_remotedio_modbus::on_unsubscribe(int mid){

}
void device_remotedio_modbus::on_log(int level, const char* str){

}
void device_remotedio_modbus::on_error(){
    
}

