
#include "moxa.io.service.task.hpp"
#include <openedge/log.hpp>


//static component instance that has only single instance
static moxaIoServiceTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new moxaIoServiceTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

/* read device configuration file */
bool moxaIoServiceTask::read_device_config(json& config){

    // 1. Device configuration
    if(config.find("device")!=config.end()){
        json device = config["device"];
        _devicename = device["name"].get<string>();
        _deviceip = device["ip"].get<string>();
        console::info("Use device {}({})", _devicename, _deviceip);

        //2.1 DI Config
        if(device.find("di")!=device.end()){
            json _di = device["di"];
            for(json::iterator itr=_di.begin(); itr!=_di.end(); ++itr){
                console::info("Config DI : {}({})", (*itr)["name"].get<std::string>(), (*itr)["pin"].get<int>());
                _di_container.insert(make_pair((*itr)["pin"].get<int>(), (*itr)["name"].get<std::string>()));
                _di_values.insert(make_pair((*itr)["name"].get<std::string>(), false));
            }
        }

        //2.2 DO Config
        if(device.find("do")!=device.end()){
            json _do = device["do"];
            for(json::iterator itr=_do.begin(); itr!=_do.end(); ++itr){
                console::info("Config DO : {}({})", (*itr)["name"].get<std::string>(), (*itr)["pin"].get<int>());
                _do_container.insert(make_pair((*itr)["pin"].get<int>(), (*itr)["name"].get<std::string>()));
                _do_values.insert(make_pair((*itr)["name"].get<std::string>(), false));
            }
        }

        //2.3 modbus config
        if(device.find("modbus_tcp")!=device.end()){
            json modbus_tcp = device["modbus_tcp"];
            if(modbus_tcp.find("port")!=modbus_tcp.end()){
                _modbus_port = modbus_tcp["port"].get<int>();
                console::info("> Modbus TCP Port : {}", _modbus_port);
            }
            if(modbus_tcp.find("di_address")!=modbus_tcp.end()){
                _di_address = modbus_tcp["di_address"].get<int>();
                console::info("> Modbus DI Address : {}", _di_address);
            }
                
            if(modbus_tcp.find("do_address")!=modbus_tcp.end()){
                _do_address = modbus_tcp["do_address"].get<int>();
                console::info("> Modbus DO Address : {}", _do_address);
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

bool moxaIoServiceTask::read_mqtt_config(json& config){
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

    return true;
}

bool moxaIoServiceTask::configure(){

    //1. read configurations
    json config = json::parse(getProfile()->get("configurations"));

    //2. read device configuration
    read_device_config(config);

    //3. read mqtt configuration
    read_mqtt_config(config);

    return true;
}

void moxaIoServiceTask::execute(){

    if(_modbus){
        //1. read DI data from modbus
        unsigned short val_di = 0x0000;
        if(modbus_read_input_registers(_modbus, _di_address, 1, &val_di)!=-1){
            for(auto& d:_di_container){
                _di_values[d.second] = static_cast<bool>(val_di&(0x0001<<d.first));
            }
        }
        else{
            console::error("Modbus Error : {}", modbus_strerror(errno));
        }

        //2. read DO data from modbus
        unsigned char val_do = 0x00;
        if(modbus_read_bits(_modbus, _do_address, 1, &val_do)!=-1){
            console::info("DO :{}", val_do);
            for(auto& d:_do_container){
                _do_values[d.second] = static_cast<bool>(val_do&(0x01<<d.first));
            }
        }

        //3. publish DI & DO data
        json pubdata;
        pubdata["di"] = _di_values;
        pubdata["do"] = _do_values;
        string str_pubdata = pubdata.dump();
        console::info("publish : {}", str_pubdata);
    }
}

void moxaIoServiceTask::cleanup(){

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

void moxaIoServiceTask::pause(){

}

void moxaIoServiceTask::resume(){

}

void moxaIoServiceTask::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Broker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);
}

void moxaIoServiceTask::on_disconnect(int rc){

}

void moxaIoServiceTask::on_publish(int mid){

}

void moxaIoServiceTask::on_message(const struct mosquitto_message* message){

    if(!_modbus){
        console::warn("Cannot handle the IO Device(Device instance is null");
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

    //2. chekc acceptable topics
    bool valid = false;
    for(string t:_mqtt_sub_topics){
        if(t==topic){
            valid = true;
            break;
        }
    }
    if(!valid){
        console::warn("<{}> topic is not acceptable", topic);
        return;
    }

    //3. parse message payload
    try {
        json msg = json::parse(strmsg);

        if(msg.find("target")!=msg.end()){
            string target = msg["target"].get<string>();
            if(target==_devicename){
                if(msg.find("command")!=msg.end()){
                    string command = msg["command"].get<string>();
                    std::transform(command.begin(), command.end(), command.begin(),[](unsigned char c){ return std::tolower(c); }); //to lower case

                    if(_service_cmd.count(command)){
                        switch(_service_cmd[command]){
                            case 1: { service_set_on(msg); } break;    //set_on service commnad
                            case 2: { service_set_off(msg); } break;   //set_off service command
                        }
                    }
                }
            }
        }
    }
    catch(json::exception& e){
        console::error("Message Error : {}", e.what());
    }

    console::info("mqtt data({}) : {}",message->payloadlen, strmsg);
}

void moxaIoServiceTask::on_subscribe(int mid, int qos_count, const int* granted_qos){
    
}

void moxaIoServiceTask::on_unsubscribe(int mid){

}

void moxaIoServiceTask::on_log(int level, const char* str){

}

void moxaIoServiceTask::on_error(){

}


void moxaIoServiceTask::service_set_on(json& msg){
    if(msg.find("name")!=msg.end()){
        string name = msg["name"].get<string>();
        std::transform(name.begin(), name.end(), name.begin(),[](unsigned char c){ return std::tolower(c); }); //to lower case
        for(auto itr = _do_container.begin(); itr != _do_container.end(); ++itr){
            if(itr->second==name){
                if(modbus_write_bit(_modbus, _do_address+itr->first, 1)==-1){
                    console::error("Modbus Error : {}", modbus_strerror(errno));
                }
                else{
                    json pub;
                    pub["do"][name] = true;
                    string str_pub = pub.dump();
                    this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(str_pub.c_str()), str_pub.c_str(), 2, false);
                    console::info("DO Published : {}", str_pub);
                }
            }
        }
    }
}

void moxaIoServiceTask::service_set_off(json& msg){
    if(msg.find("name")!=msg.end()){
        string name = msg["name"].get<string>();
        std::transform(name.begin(), name.end(), name.begin(),[](unsigned char c){ return std::tolower(c); }); //to lower case
        for(auto itr = _do_container.begin(); itr != _do_container.end(); ++itr){
            if(itr->second==name){
                if(modbus_write_bit(_modbus, _do_address+itr->first, 0)==-1){
                    console::error("Modbus Error : {}", modbus_strerror(errno));
                }
                else{
                    json pub;
                    pub["do"][name] = true;
                    string str_pub = pub.dump();
                    this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(str_pub.c_str()), str_pub.c_str(), 2, false);
                    console::info("DO Published : {}", str_pub);
                }
            }
        }
    }
}