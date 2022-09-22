
#include "device.ulory.control.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static device_ulory_control* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new device_ulory_control(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void device_ulory_control::execute(){

    const int buffer_size = 1024*8;

    if(_device->is_open()){
        char* buffer = new char[buffer_size];
        int bytes = _device->read(buffer, sizeof(char)*buffer_size);


        //protocol process
        

        delete []buffer;
    }
    else {
        console::error("Device {} is not opened.", _port);
    }
    
}

void device_ulory_control::stop(){

}

bool device_ulory_control::configure(){
    try {
        const json& profile = this->get_profile()->raw();

        if(profile.contains(PROFILE_CONFIGURATIONS_KEY)){
            json config = profile[PROFILE_CONFIGURATIONS_KEY];

            if(config.contains("device")){
                json device_param = config["device"];

                _port = device_param["port"].get<string>();
                _baudrate = device_param["baudrate"].get<int>();
                _timeout_s = device_param["timeout"].get<int>();

                console::info("> Device Port : {}", _port);
                console::info("> Device Buadrate : {}", _baudrate);
                console::info("> Communication Timeout(sec) : {}", _timeout_s);

                if(!_device){
                    _device = new oe::device::systembase::ulory(_port.c_str(), _baudrate, _timeout_s);
                    _device->open();
                }
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

void device_ulory_control::cleanup(){
    if(_device){
        _device->close();
        delete _device;
    }

    /* mqtt connection close */
    this->mosqpp::mosquittopp::disconnect();
    this->mosqpp::mosquittopp::loop_stop();
    mosqpp::lib_cleanup();

}

void device_ulory_control::pause(){
    
}

void device_ulory_control::resume(){
    
}


void device_ulory_control::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Broker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);

}
void device_ulory_control::on_disconnect(int rc){

}
void device_ulory_control::on_publish(int mid){

}
void device_ulory_control::on_message(const struct mosquitto_message* message){

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
void device_ulory_control::on_subscribe(int mid, int qos_count, const int* granted_qos){

}
void device_ulory_control::on_unsubscribe(int mid){

}
void device_ulory_control::on_log(int level, const char* str){

}
void device_ulory_control::on_error(){
    
}
