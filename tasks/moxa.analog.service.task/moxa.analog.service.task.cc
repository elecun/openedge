
#include "moxa.analog.service.task.hpp"
#include <openedge/log.hpp>


//static component instance that has only single instance
static moxaAnalogServiceTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new moxaAnalogServiceTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

/* read device configuration file */
bool moxaAnalogServiceTask::read_device_config(json& config){

    // 1. Device configuration
    if(config.find("device")!=config.end()){
        json device = config["device"];
        _devicename = device["name"].get<string>();
        _deviceip = device["ip"].get<string>();
        console::info("Use device {}({})", _devicename, _deviceip);

        //2.1 DI Config
        if(device.find("ai")!=device.end()){
            json _ai = device["ai"];
            for(json::iterator itr=_ai.begin(); itr!=_ai.end(); ++itr){
                console::info("Config AI : {}({})", (*itr)["name"].get<std::string>(), (*itr)["pin"].get<int>());
                _ai_container.insert(make_pair((*itr)["pin"].get<int>(), (*itr)["name"].get<std::string>()));
                _ai_value_container.insert(make_pair((*itr)["name"].get<std::string>(), false));
            }
        }

        //2.2 modbus config
        if(device.find("modbus_tcp")!=device.end()){
            json modbus_tcp = device["modbus_tcp"];
            if(modbus_tcp.find("port")!=modbus_tcp.end()){
                _modbus_port = modbus_tcp["port"].get<int>();
                console::info("> Modbus TCP Port : {}", _modbus_port);
            }
            if(modbus_tcp.find("ai_address")!=modbus_tcp.end()){
                _ai_address = modbus_tcp["ai_address"].get<int>();
                console::info("> Modbus AI Address : {}", _ai_address);
            }

            // modbus connection
            if(!_modbus){
                _modbus = modbus_new_tcp(_deviceip.c_str(), _modbus_port);
                if(!_modbus){
                    console::error("Unable to create the modbus context");
                    return false;
                }

                if(modbus_connect(_modbus)==-1){
                    console::error("Device connection failed : {}", modbus_strerror(errno));
                    modbus_free(_modbus);
                    return false;
                }
                else
                    console::info("Successfully connected to Modbus TCP Server");
            }
        }
        else
            return false;
    }
    else
        return false;

    return true;
}

bool moxaAnalogServiceTask::read_mqtt_config(json& config){
    if(config.find("mqtt")!=config.end()){
        json mqtt_param = config["mqtt"];
        if(mqtt_param.find("broker")!=mqtt_param.end()) _broker_address = mqtt_param["broker"].get<string>();
        if(mqtt_param.find("port")!=mqtt_param.end()) _broker_port = mqtt_param["port"].get<int>();
        if(mqtt_param.find("pub_topic")!=mqtt_param.end()) _mqtt_pub_topic = mqtt_param["pub_topic"].get<string>();
        if(mqtt_param.find("pub_qos")!=mqtt_param.end()) _mqtt_pub_qos = mqtt_param["pub_qos"].get<int>();
        if(mqtt_param.find("keep_alive")!=mqtt_param.end()) _mqtt_keep_alive = mqtt_param["keep_alive"].get<int>();
        if(mqtt_param.find("sub_topic")!=mqtt_param.end()){
            for(json::iterator itr=mqtt_param["sub_topic"].begin(); itr!=mqtt_param["sub_topic"].end(); ++itr){
                _mqtt_sub_topics.emplace_back(*itr);
            }
        } 
        if(mqtt_param.find("method")!=mqtt_param.end()){
            if(!mqtt_param["method"].get<string>().compare("on_update")) _pub_method = PUBLISH_METHOD::ON_UPDATE;
            else if(!mqtt_param["method"].get<string>().compare("on_ai_change")) _pub_method = PUBLISH_METHOD::ON_AI_CHANGE;
        }

        console::info("> set MQTT Broker : {}", _broker_address);
        console::info("> set MQTT Port : {}", _broker_port);
        console::info("> set MQTT Pub. Topic : {}", _mqtt_pub_topic);
        console::info("> set MQTT Pub. QoS : {}", _mqtt_pub_qos);
        console::info("> set MQTT Keep-alive : {}", _mqtt_keep_alive);
        console::info("> set MQTT Pub. Method : {}", _pub_method);

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

    return true;
}

bool moxaAnalogServiceTask::configure(){

    //1. read configurations
    json config = json::parse(getProfile()->get("configurations"));

    //2. read device configuration
    read_device_config(config);

    //3. read mqtt configuration
    read_mqtt_config(config);

    return true;
}

void moxaAnalogServiceTask::execute(){

    if(_modbus){
        unsigned short _prev_ai_values[CHANNELS*2] = {0x0000, };
        memcpy(_prev_ai_values, _ai_values, sizeof(unsigned short)*CHANNELS*2);

        //1. read DI data
        if(modbus_read_input_registers(_modbus, _ai_address, CHANNELS*2, _ai_values)!=-1){
            
            for(int i=0; i<CHANNELS; i++){
                u.l = ((unsigned long)_ai_values[i*2+1]<<16 | _ai_values[i*2]);
                _ai_value_container[_ai_container[i]] = u.f;
            }
        }
        else{
            console::error("Modbus Error : {}", modbus_strerror(errno));
        }


        //2. publish AI data
        if(_pub_method==PUBLISH_METHOD::ON_UPDATE){
            json pubdata;
            pubdata["ai"] = _ai_value_container;
            string str_pubdata = pubdata.dump();
            this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(str_pubdata.c_str()), str_pubdata.c_str(), 2, false);   
            //console::info("Publish Data : {}", str_pubdata);
        }
        else if(_pub_method==PUBLISH_METHOD::ON_AI_CHANGE){
            bool changed = false;
            for(int i=0;i<CHANNELS;i++){
                u.l = ((unsigned long)_prev_ai_values[i*2+1]<<16 | _prev_ai_values[i*2]);
                if(u.f!=_ai_value_container[_ai_container[i]]){
                    changed = true;
                    //console::info("diff {} : {}, {}", i, u.f, _ai_value_container[_ai_container[i]]);
                    break;
                }
            }

            if(changed){
                json pubdata;
                pubdata["ai"] = _ai_value_container;
                string str_pubdata = pubdata.dump();
                this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(str_pubdata.c_str()), str_pubdata.c_str(), 2, false);   
                //console::info("Publish Data : {}", str_pubdata);
            }
        }
        
    }
}

void moxaAnalogServiceTask::cleanup(){

    // 1. modbus conenction close
    if(_modbus){
        modbus_close(_modbus);
        modbus_free(_modbus);
        console::info("Modbus connection closed");
    }

    // 2. mqtt connection close
    this->mosqpp::mosquittopp::disconnect();
    this->mosqpp::mosquittopp::loop_stop();
    mosqpp::lib_cleanup();
}

void moxaAnalogServiceTask::pause(){

}

void moxaAnalogServiceTask::resume(){

}

void moxaAnalogServiceTask::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Broker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);
}

void moxaAnalogServiceTask::on_disconnect(int rc){

}

void moxaAnalogServiceTask::on_publish(int mid){

}

void moxaAnalogServiceTask::on_message(const struct mosquitto_message* message){

    if(!_modbus){
        console::error("Cannot handle the IO Device(Device instance is null");
        return;
    }
    
    //1. receive payload
    #define MAX_BUFFER_SIZE     4096
    char* buffer = new char[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(char)*MAX_BUFFER_SIZE);
    memcpy(buffer, message->payload, sizeof(char)*message->payloadlen);
    string topic(message->topic);
    string strmsg(buffer);
    delete []buffer;
}

void moxaAnalogServiceTask::on_subscribe(int mid, int qos_count, const int* granted_qos){
    
}

void moxaAnalogServiceTask::on_unsubscribe(int mid){

}

void moxaAnalogServiceTask::on_log(int level, const char* str){

}

void moxaAnalogServiceTask::on_error(){

}

