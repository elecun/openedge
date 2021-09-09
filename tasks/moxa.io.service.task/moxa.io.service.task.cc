
#include "moxa.io.service.task.hpp"
#include <openedge/log.hpp>
#include <3rdparty/libmodbus/modbus.h>


//static component instance that has only single instance
static moxaIoServiceTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new moxaIoServiceTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

//static instance
static modbus_t* _modbus = nullptr;

bool moxaIoServiceTask::configure(){

    //1. read configurations
    json config = json::parse(getProfile()->get("configurations"));

    // 2. Device configuration
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
            }
        }
    }

    //3. MQTT Config
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

void moxaIoServiceTask::execute(){

    //1. read IO from modbus
    if(_modbus){
        unsigned short val_di = 0x0000;
        map<string, bool> _di_values_temp;
        if(modbus_read_input_registers(_modbus, 48, 1, &val_di)!=-1){
            for(auto& di:_di_container){
                _di_values_temp[di.second] = static_cast<bool>(val_di&(0x0001<<di.first));
            }
        }
        else{
            console::info("Modbus Error : {}", modbus_strerror(errno));
        }

        // check the value changed
        for(auto& di:_di_container){
            if(_di_values_temp[di.second]==_di_values[di.second]){
                _di_values_temp.erase(_di_values_temp.find(di.second));
            }
            else{
                _di_values[di.second] = _di_values_temp[di.second]; //update
            }
        }

        //publish changed
        if(_di_values_temp.size()){
            json pub;
            pub["di"] = _di_values_temp;
            string str_pub = pub.dump();
            this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(str_pub.c_str()), str_pub.c_str(), 2, false);
            console::info("publish : {}", str_pub);
        }
    }
}

void moxaIoServiceTask::cleanup(){

    // 1. modbus conenction close
    if(_modbus){
        modbus_close(_modbus);
        modbus_free(_modbus);
    }

    // 2. mqtt connection close
    this->disconnect();
    this->loop_stop();
    mosqpp::lib_cleanup();
}

void moxaIoServiceTask::pause(){

}

void moxaIoServiceTask::resume(){

}

void moxaIoServiceTask::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Brocker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);
}

void moxaIoServiceTask::on_disconnect(int rc){

}

void moxaIoServiceTask::on_publish(int mid){

}

void moxaIoServiceTask::on_message(const struct mosquitto_message* message){
    #define MAX_BUFFER_SIZE     4096

    char* buffer = new char[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(char)*MAX_BUFFER_SIZE);
    memcpy(buffer, message->payload, sizeof(char)*message->payloadlen);
    string strmsg = buffer;
    delete []buffer;

    // try{
    //     json msg = json::parse(strmsg);
    //     DKM_DX3000* motor = dynamic_cast<DKM_DX3000*>(_controller);
    //     if(!motor)
    //         return;

    //     //command
    //     if(msg.find("command")!=msg.end()){
    //         string command = msg["command"].get<string>();
    //         std::transform(command.begin(), command.end(), command.begin(),[](unsigned char c){ return std::tolower(c); }); //to lower case

    //         if(_dx_command.count(command)){
    //             switch(_dx_command[command]){
    //                 case 1: { //move cw
    //                     motor->move(DKM_DX3000::DIRECTION::CW);
    //                     console::info("Motor moves forward(CW)");
    //                 } break;
    //                 case 2: { //move ccw
    //                     motor->move(DKM_DX3000::DIRECTION::CCW);
    //                     console::info("Motor moves backward(CCW)");
    //                 } break;
    //                 case 3: { //stop
    //                     motor->stop();
    //                 } break;
    //                 case 4: { //rpm
    //                     int value = msg["value"].get<int>();
    //                     motor->set_rpm((unsigned short)value);
    //                     console::info("motor rpm set : {}", value);
    //                 } break;
    //             }
    //         }
    //     }
    // }
    // catch(json::exception& e){
    //     console::error("Message Error : {}", e.what());
    // }
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
