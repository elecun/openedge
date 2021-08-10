
#include "agw.manage.task.hpp"
#include <openedge/log.hpp>

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

    if(this->mqttConnector::is_connected()){
        string topic = "aop/test";
        string msg = "test";
        this->mqttConnector::publish(nullptr, topic.c_str(), msg.size(), msg.c_str(), 2, false);
    }


    // if(this->mqttConnector::is_connected()) {
    //     int rc = this->loop();
    //     if(rc!=MOSQ_ERR_SUCCESS){
    //         this->mqttConnector::reconnect();
    //         console::info("try reconnect...");
    //     }

    //     string topic = "aop/test";
    //     string msg = "test";
    //     this->mqttConnector::publish(nullptr, topic.c_str(), msg.size(), msg.c_str(), 2, false);
          
    // }

    // if(!this->mqttConnector::is_connected()) {
    //     if(int rc = this->mqttConnector::reconnect()!=MOSQ_ERR_SUCCESS)
    //         console::info("try reconnecting to broker...");
    //     else{
    //         console::warn("{}", mosqpp::strerror(rc));
    //         // periodically publish the system resource
    //         string topic = "aop/test";
    //         string msg = "test";
    //         this->mqttConnector::publish(nullptr, topic.c_str(), msg.size(), msg.c_str(), 2, false);
    //         this->mqttConnector::loop_write();
    //     }
    // }
    // else {
    //     // periodically publish the system resource
    //     string topic = "aop/test";
    //     string msg = "test";
    //     this->mqttConnector::publish(nullptr, topic.c_str(), msg.size(), msg.c_str(), 2, false);
    // }
    
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
