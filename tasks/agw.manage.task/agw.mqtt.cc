
#include "agw.manage.task.hpp"
#include <openedge/log.hpp>

bool mqttConnector::open(json param){

    if(const int ret = mosqpp::lib_init()!=MOSQ_ERR_SUCCESS){
        console::error("({}){}", ret, mosqpp::strerror(ret));
        return false;
    }

    if(param.find("broker")!=param.end()) _broker_address = param["broker"].get<string>();
    if(param.find("port")!=param.end()) _broker_port = param["port"].get<int>();
    if(param.find("pub_topic")!=param.end()) _mqtt_pub_topic = param["pub_topic"].get<string>();
    if(param.find("pub_qos")!=param.end()) _mqtt_pub_qos = param["pub_qos"].get<int>();
    if(param.find("keep_alive")!=param.end()) _mqtt_keep_alive = param["keep_alive"].get<int>();
    if(param.find("sub_topic")!=param.end()){
        for(json::iterator itr=param["sub_topic"].begin(); itr!=param["sub_topic"].end(); ++itr){
            _mqtt_sub_topics.emplace_back(*itr);
        }
    } 

    console::info("> set MQTT Broker : {}", _broker_address);
    console::info("> set MQTT Port : {}", _broker_port);
    console::info("> set MQTT Pub. Topic : {}", _mqtt_pub_topic);
    console::info("> set MQTT Pub. QoS : {}", _mqtt_pub_qos);
    console::info("> set MQTT Keep-alive : {}", _mqtt_keep_alive);

    //connect to MQTT broker
    int rc = this->connect_async(_broker_address.c_str(), _broker_port, _mqtt_keep_alive);
    if(rc==MOSQ_ERR_SUCCESS){
        const int startrc = this->loop_start();
        if(startrc!=MOSQ_ERR_SUCCESS){
            console::info("connection failed : {}", rc, mosqpp::strerror(rc));
        }

        for(string topic:_mqtt_sub_topics){
            this->subscribe(nullptr, topic.c_str(), 2);
            console::info("> set MQTT Sub. Topic : {}", topic);
        }
        
        return true;
    }
    else {
        console::warn("MQTT broker connection failed({}) : {}", rc, mosqpp::strerror(rc));
    }

    return false;
}


void mqttConnector::on_connect(int rc){
    _connected = true;
    console::info("Connected to MQTT Broker");
}

void mqttConnector::on_disconnect(int rc){
    _connected = false;
    console::info("Disconnected to MQTT Broker");
}

void mqttConnector::on_publish(int mid){

}

void mqttConnector::on_message(const struct mosquitto_message* message){

}

void mqttConnector::on_subscribe(int mid, int qos_count, const int* granted_qos){

}

void mqttConnector::on_unsubscribe(int mid){

}

void mqttConnector::on_log(int level, const char* str){
    console::info("MQTT Log({}) : {}", level, str);
}

void mqttConnector::on_error(){

} 

void mqttConnector::close()
{
    this->disconnect();
    this->loop_stop();
    mosqpp::lib_cleanup();
}

int mqttConnector::reconnect(){
    return this->reconnect_async();
}