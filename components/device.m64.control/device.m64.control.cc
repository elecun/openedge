
#include "device.m64.control.hpp"
#include <openedge/log.hpp>
#include <bits/stdc++.h>

//static component instance that has only single instance
static device_m64_control* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new device_m64_control(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void device_m64_control::execute(){
    const int buffer_size = 1024*8;
    const int packet_size = 19;

    if(_device->is_open()){
        char* buffer = new char[buffer_size];
        int bytes = _device->read(buffer, sizeof(char)*buffer_size);

        //1. append to q
        _dq.insert(_dq.end(), buffer, buffer + sizeof(char)*bytes);
        //console::info("insert to dq : {}", _dq.size());

        if(_dq.size()<packet_size)
            return;

        //2. alignment (start with 0x77)
        while(1){
            if(_dq[0]==_M64_START_ && _dq[1]==_M64_RESPONSE_ && _dq[2]==_M64_RESP_GOT_PACKET_) break;
            else _dq.pop_front();

            if(_dq.size()<4) break;
        }

        //show raw data
        // string test;
        // for(char q:_dq){
        //     test+=fmt::format("{0:x} ",q);
        // }
        // console::info(test);

        if(_dq.size()>=packet_size){

            string str_data = "";
            if(_dq[0]==_M64_START_ && _dq[1]==_M64_RESPONSE_ && _dq[2]==_M64_RESP_GOT_PACKET_ && _dq[17]==0x0d && _dq[18]==0x0a){

                //parse counter
                //3. data parse
                //str_data += fmt::format("({})", _dq[4]);

                double water_temp = (double)((int)((_dq[7]<<8)+_dq[6]))/100;
                double water_depth = (double)((int)((_dq[9]<<8)+_dq[8]))/100;

                console::info("water depth : {}\t water temperature : {}", water_depth, water_temp);

                /* publish data */
                if(_mqtt_connected){
                    json pubdata;
                    pubdata["water_depth"] = water_depth;
                    pubdata["water_temperature"] = water_temp;
                    string str_pubdata = pubdata.dump();
                    this->publish(nullptr, _pub_topic.c_str(), strlen(str_pubdata.c_str()), str_pubdata.c_str(), _pub_qos, false);
                }               

                //erase data
                for(int i=0;i<packet_size;i++){
                    _dq.pop_front();
                }
            }
        }
        
        delete []buffer;
    }
    else {
        console::error("Device {} is not opened.", _port);
    }
    
}

void device_m64_control::stop(){

}

bool device_m64_control::configure(){
    try {
        const json& profile = this->get_profile()->raw();

        if(profile.contains(PROFILE_CONFIGURATIONS_KEY)){
            json config = profile[PROFILE_CONFIGURATIONS_KEY];

            /* UART device configurations  */
            if(config.contains("device")){
                json device_param = config["device"];

                _port = device_param["port"].get<string>();
                _baudrate = device_param["baudrate"].get<int>();
                _timeout_s = device_param["timeout"].get<int>();

                console::info("> Device Port : {}", _port);
                console::info("> Device Buadrate : {}", _baudrate);
                console::info("> Communication Timeout(sec) : {}", _timeout_s);

                if(!_device){
                    _device = new device::waterlink::m64(_port.c_str(), _baudrate);
                    _device->open();
                }
            }

            if(config.contains("mqtt")){
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
        }
        else{
            console::error("Device Configurations does not exist");
            return false;
        }
    }
    catch(const json::exception& e){
        console::error("Profile read/access error : {}", e.what());
        return false;
    }

    return true;
}

void device_m64_control::cleanup(){

    /* mqtt connection close */
    this->mosqpp::mosquittopp::disconnect();
    this->mosqpp::mosquittopp::loop_stop();
    mosqpp::lib_cleanup();

}

void device_m64_control::pause(){
    
}

void device_m64_control::resume(){
    
}


void device_m64_control::on_connect(int rc){
    _mqtt_connected = true;

}
void device_m64_control::on_disconnect(int rc){
    _mqtt_connected = false;
}
void device_m64_control::on_publish(int mid){

}
void device_m64_control::on_message(const struct mosquitto_message* message){

    #define MAX_BUFFER_SIZE     1024
    char* buffer = new char[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(char)*MAX_BUFFER_SIZE);
    memcpy(buffer, message->payload, sizeof(char)*message->payloadlen);
    string topic(message->topic);
    string strmsg = buffer;
    delete []buffer;

    try{
        json msg = json::parse(strmsg);

    }
    catch(json::exception& e){
        console::error("Message Error : {}", e.what());
    }
    console::info("mqtt data({}) : {}",message->payloadlen, strmsg);

}
void device_m64_control::on_subscribe(int mid, int qos_count, const int* granted_qos){

}
void device_m64_control::on_unsubscribe(int mid){

}
void device_m64_control::on_log(int level, const char* str){

}
void device_m64_control::on_error(){
    
}
