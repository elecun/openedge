
#include "agw.manage.task.hpp"
#include <openedge/log.hpp>
#include <openedge/sys/system.hpp>

//static component instance that has only single instance
static agwManageTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new agwManageTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool agwManageTask::configure(){

    json config = json::parse(getProfile()->get("configurations"));
    
    if(config.find("mqtt")!=config.end()){
        json mqtt_param = config["mqtt"];
        if(!this->mqttConnector::open(mqtt_param)){
            console::info("MQTT connector open failed");
        }
    }

    return true;
}

void agwManageTask::execute(){

    //1. periodically transmit the system resources use
    if(this->mqttConnector::is_connected()) {
        string msg = this_system->getStats().dump();
        this->mqttConnector::publish(nullptr, this->_mqtt_pub_topic.c_str(), msg.size(), msg.c_str(), 2, false);
    }
    else {
        if(const int rc = this->mqttConnector::reconnect_async()!=MOSQ_ERR_SUCCESS){
            console::warn("Reconnection failed({}) : {}", rc, mosqpp::strerror(rc));
        }
    }

    //2. periodically transmit the CPU usage of openedge processes
    
}

void agwManageTask::cleanup(){
    this->mqttConnector::close();
    console::info("call agwManageTask cleanup()");
}

void agwManageTask::pause(){
    console::info("call agwManageTask pause()");
}

void agwManageTask::resume(){
    console::info("call agwManageTask resume()");
}

void agwManageTask::on_message(const struct mosquitto_message* message){
    #define MAX_BUFFER_SIZE     4096
    char* buffer = new char[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(char)*MAX_BUFFER_SIZE);
    memcpy(buffer, message->payload, sizeof(char)*message->payloadlen);
    string strmsg = buffer;

    console::info("message : {}", strmsg);

    delete []buffer;
}
