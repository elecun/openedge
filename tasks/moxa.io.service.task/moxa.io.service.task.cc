
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
                _di_value_container.insert(make_pair((*itr)["name"].get<std::string>(), false));
            }
        }

        //2.2 DO Config
        if(device.find("do")!=device.end()){
            json _do = device["do"];
            for(json::iterator itr=_do.begin(); itr!=_do.end(); ++itr){
                console::info("Config DO : {}({})", (*itr)["name"].get<std::string>(), (*itr)["pin"].get<int>());
                _do_container.insert(make_pair((*itr)["pin"].get<int>(), (*itr)["name"].get<std::string>()));
                _do_value_container.insert(make_pair((*itr)["name"].get<std::string>(), false));
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
        if(mqtt_param.find("method")!=mqtt_param.end()){
            if(!mqtt_param["method"].get<string>().compare("on_update")) _pub_method = PUBLISH_METHOD::ON_UPDATE;
            else if(!mqtt_param["method"].get<string>().compare("on_di_change")) _pub_method = PUBLISH_METHOD::ON_DI_CHANGE;
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
        unsigned short _prev_di_values = _di_values;

        //1. read DI data
        if(modbus_read_input_registers(_modbus, _di_address, 1, &_di_values)!=-1){
            for(auto& d:_di_container){
                _di_value_container[d.second] = static_cast<bool>(_di_values&(0x0001<<d.first));
            }
        }
        else{
            console::error("Modbus Error : {}", modbus_strerror(errno));
        }

        // 2. read DO data
        if(modbus_read_registers(_modbus, _do_address, 1, &_do_values)!=-1){
            for(auto& d:_do_container){
                _do_value_container[d.second] = static_cast<bool>(_do_values&(0x0001<<d.first));
            }
        }
        else{
            console::error("Modbus Error : {}", modbus_strerror(errno));
        }


        //3. publish DI & DO data
        if(_pub_method==PUBLISH_METHOD::ON_UPDATE){
            json pubdata;
            pubdata["di"] = _di_value_container;
            pubdata["do"] = _do_value_container;
            string str_pubdata = pubdata.dump();
            this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(str_pubdata.c_str()), str_pubdata.c_str(), 2, false);   
            console::info("Publish Data : {}", str_pubdata);
        }
        else if(_pub_method==PUBLISH_METHOD::ON_DI_CHANGE){
            if(_prev_di_values!=_di_values){
                json pubdata;
                pubdata["di"] = _di_value_container;
                pubdata["do"] = _do_value_container;
                string str_pubdata = pubdata.dump();
                this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(str_pubdata.c_str()), str_pubdata.c_str(), 2, false);   
                console::info("Publish Data : {}", str_pubdata);
            }
        }
        
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

    
    //2. parse message payload
    try {
        json msg = json::parse(strmsg);
        if(!topic.compare("aop/uvlc/1/io/control")){
            if(msg.find("operator")!=msg.end()){
                string op = msg["operator"];
                if(!op.compare("OR") && msg.find("DO")!=msg.end()){
                    set_DO_or(msg["DO"].get<unsigned short>());
                }
            }
            else {
                if(msg.find("DO")!=msg.end()){
                    set_DO(msg["DO"].get<unsigned short>());
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

void moxaIoServiceTask::set_DO(unsigned short value){
    if(_modbus){
        if(modbus_write_registers(_modbus, _do_address, 1, &value)==-1)
            console::error("Modbus Error : {}", modbus_strerror(errno));
    }
    else {
        console::error("Unable to use MODBUS");
    }
}

void moxaIoServiceTask::set_DO_or(unsigned short value){
    if(_modbus){
        unsigned short _val = _do_values|value;
        if(modbus_write_registers(_modbus, _do_address, 1, &_val)==-1)
            console::error("Modbus Error : {}", modbus_strerror(errno));
    }
    else {
        console::error("Unable to use MODBUS");
    }
}