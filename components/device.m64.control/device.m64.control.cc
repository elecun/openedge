
#include "device.m64.control.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static device_m64_control* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new device_m64_control(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void device_m64_control::execute(){

    
}

void device_m64_control::stop(){

}

bool device_m64_control::configure(){
    try {
        const json& profile = this->get_profile()->raw();

        if(!profile.contains("device")){
            console::error("Device Configurations does not exist");
            return false;
        }

        json device_param = profile["device"];
        _port = device_param["port"].get<string>();
        _baudrate = device_param["baudrate"].get<int>();
        _timeout_s = device_param["timeout"].get<double>();

        console::info("> Device Port : {}", _port);
        console::info("> Device Buadrate : {}", _baudrate);
        console::info("> Communication Timeout(sec) : {}", _timeout_s);


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
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Broker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);

}
void device_m64_control::on_disconnect(int rc){

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
