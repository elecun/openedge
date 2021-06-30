
#include "uvlc.control.task.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static uvlcControlTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new uvlcControlTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool uvlcControlTask::configure(){

    //initialize mosquitto
    if(const int ret = mosqpp::lib_init()!=MOSQ_ERR_SUCCESS){
        console::error("({}){}", ret, mosqpp::strerror(ret));
        return false;
    }

    //read configuration from profile
    json config = json::parse(getProfile()->get("configurations"));

    //read MQTT parameters & connect to the broker
    if(config.find("mqtt")!=config.end()){
        json mqtt_param = config["mqtt"];
        if(mqtt_param.find("broker")!=mqtt_param.end()) _mqtt_broker = mqtt_param["broker"].get<string>();
        if(mqtt_param.find("port")!=mqtt_param.end()) _mqtt_port = mqtt_param["port"].get<int>();
        if(mqtt_param.find("pub_topic")!=mqtt_param.end()) _mqtt_pub_topic = mqtt_param["pub_topic"].get<string>();
        if(mqtt_param.find("pub_qos")!=mqtt_param.end()) _mqtt_pub_qos = mqtt_param["pub_qos"].get<int>();
        if(mqtt_param.find("keep_alive")!=mqtt_param.end()) _mqtt_keep_alive = mqtt_param["keep_alive"].get<int>();
        if(mqtt_param.find("sub_topic")!=mqtt_param.end()){
            for(json::iterator itr=mqtt_param["sub_topic"].begin(); itr!=mqtt_param["sub_topic"].end(); ++itr){
                _mqtt_sub_topics.emplace_back(*itr);
            }
        } 

        console::info("> set MQTT Broker : {}", _mqtt_broker);
        console::info("> set MQTT Port : {}", _mqtt_port);
        console::info("> set MQTT Pub. Topic : {}", _mqtt_pub_topic);
        console::info("> set MQTT Pub. QoS : {}", _mqtt_pub_qos);
        console::info("> set MQTT Keep-alive : {}", _mqtt_keep_alive);

        //connect to MQTT broker
        if(const int conret = this->connect_async(_mqtt_broker.c_str(), _mqtt_port, _mqtt_keep_alive)==MOSQ_ERR_SUCCESS){
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

    //read uvlc configuration from profile
    if(config.find("uvlc")!=config.end()){
        json uvlc_param = config["uvlc"];
        if(uvlc_param.find("limit_sensor")!=uvlc_param.end()){
            string id = uvlc_param["limit_sensor"].get<string>();
            _limit_id = std::stoul(id.c_str(), nullptr, 16);
            console::info("> set UVLC Limit Sensor CAN ID : {}", _limit_id);
        }

        if(uvlc_param.find("intensity_sensor")!=uvlc_param.end()){
            for(json::iterator itr=uvlc_param["intensity_sensor"].begin(); itr!=uvlc_param["intensity_sensor"].end(); ++itr){
                string value = itr->get<string>();
                _intensity_id[value] = (unsigned short)std::stoul(value.c_str(), nullptr, 16);
                console::info("> set UVLC Intensity Sensor CAN ID : {}", _limit_id);
            }
        }

    }
    
    return true;
}

void uvlcControlTask::execute(){
    //publish control event (control mode,)
    console::info("uvlc control task execution");

    for(map<string, unsigned short>::iterator itr = _intensity_id.begin(); itr!=_intensity_id.end(); ++itr){
        // _intensity_value[itr->second] = 
    }
    
}

void uvlcControlTask::cleanup(){
    //MQTT connection close
    this->disconnect();
    this->loop_stop();
    mosqpp::lib_cleanup();
}

void uvlcControlTask::pause(){

}

void uvlcControlTask::resume(){

}

void uvlcControlTask::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Brocker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);
}

void uvlcControlTask::on_disconnect(int rc){

}

void uvlcControlTask::on_publish(int mid){

}

void uvlcControlTask::on_message(const struct mosquitto_message* message){

    #define MAX_BUFFER_SIZE     4096
    char* buffer = new char[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(char)*MAX_BUFFER_SIZE);
    memcpy(buffer, message->payload, sizeof(char)*message->payloadlen);
    string strmsg = buffer;
    delete []buffer;

    string msg_topic(message->topic);
    if(!msg_topic.compare("aop/uvlc/sensor")){ //sensor data

        try {
            json msg = json::parse(strmsg);
            if(msg.find("id")!=msg.end()){

                console::info(msg.dump());
            }
        }
        catch(json::exception& e){
            console::error("Message Error : {}", e.what());
        }
    }
}

void uvlcControlTask::on_subscribe(int mid, int qos_count, const int* granted_qos){
    
}

void uvlcControlTask::on_unsubscribe(int mid){

}

void uvlcControlTask::on_log(int level, const char* str){

}

void uvlcControlTask::on_error(){

}
