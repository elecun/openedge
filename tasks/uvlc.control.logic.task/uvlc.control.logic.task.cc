
#include "uvlc.control.logic.task.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static uvlcControLogiclTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new uvlcControLogiclTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool uvlcControLogiclTask::configure(){

    json config = json::parse(getProfile()->get("configurations"));

    // MQTT configuration
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
            else if(!mqtt_param["method"].get<string>().compare("on_ai_change")) _pub_method = PUBLISH_METHOD::ON_CHANGE;
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

    // read parameter
    if(config.find("parameter")!=config.end()){
        json uvlc_param = config["parameter"];

        if(uvlc_param.find("l_proximity_io")!=uvlc_param.end()){
            _l_proximity_io = uvlc_param["l_proximity_io"].get<string>();
            console::info("> UVLC Proximity IO(Left) : {}", _l_proximity_io);
        }
        if(uvlc_param.find("r_proximity_io")!=uvlc_param.end()){
            _r_proximity_io = uvlc_param["r_proximity_io"].get<string>();
            console::info("> UVLC Proximity IO(Right) : {}", _r_proximity_io);
        }
        if(uvlc_param.find("wipe_forward_io")!=uvlc_param.end()){
            _wipe_forward_io = uvlc_param["wipe_forward_io"].get<string>();
            console::info("> UVLC WIPE FORWARD I/O : {}", _wipe_forward_io);
        }
        if(uvlc_param.find("wipe_reverse_io")!=uvlc_param.end()){
            _wipe_reverse_io = uvlc_param["wipe_reverse_io"].get<string>();
            console::info("> UVLC WIPE REVERSE I/O : {}", _wipe_reverse_io);
        }
        if(uvlc_param.find("wipe_once_io")!=uvlc_param.end()){
            _wipe_once_io = uvlc_param["wipe_once_io"].get<string>();
            console::info("> UVLC WIPE ONCE I/O : {}", _wipe_once_io);
        }
        if(uvlc_param.find("stop_io")!=uvlc_param.end()){
            _stop_io = uvlc_param["stop_io"].get<string>();
            console::info("> UVLC WIPE STOP I/O : {}", _stop_io);
        }
        
    }
    
    return true;
}

void uvlcControLogiclTask::execute(){
    
    check_stop_enable(_stop_io_value);
    
}

void uvlcControLogiclTask::cleanup(){

    json pubdata;
    pubdata["command"] = "stop";
    string str_pubdata = pubdata.dump();
    this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(str_pubdata.c_str()), str_pubdata.c_str(), 2, false);   
    
    //MQTT connection close
    this->disconnect();
    this->loop_stop();
    mosqpp::lib_cleanup();
}

void uvlcControLogiclTask::pause(){

}

void uvlcControLogiclTask::resume(){

}

void uvlcControLogiclTask::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Brocker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);
}

void uvlcControLogiclTask::on_disconnect(int rc){

}

void uvlcControLogiclTask::on_publish(int mid){

}

void uvlcControLogiclTask::on_message(const struct mosquitto_message* message){

    // 1. read buffer
    #define MAX_BUFFER_SIZE     4096
    char* buffer = new char[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(char)*MAX_BUFFER_SIZE);
    memcpy(buffer, message->payload, sizeof(char)*message->payloadlen);
    string topic(message->topic);
    string strmsg = buffer;
    delete []buffer;

    //2. parse message payload
    try {
        json msg = json::parse(strmsg);
        // if(msg.find("di")!=msg.end()){
        //     json di_values = msg["di"];
        //     if(di_values.find(_stop_io))
        // }

    }
    catch(json::exception& e){
        console::error("Message Error : {}", e.what());
    }

    
}

void uvlcControLogiclTask::on_subscribe(int mid, int qos_count, const int* granted_qos){
    
}

void uvlcControLogiclTask::on_unsubscribe(int mid){

}

void uvlcControLogiclTask::on_log(int level, const char* str){

}

void uvlcControLogiclTask::on_error(){

}

/* check stop io button (low edge active) */
bool uvlcControLogiclTask::check_stop_enable(const bool value){
    static bool prev_val = false;
    bool res = false;
    if(prev_val && !value){
        res = true;
    }
    prev_val = value;
    return res;
}
/* check left proximity sensor (high edge active) */
bool uvlcControLogiclTask::check_l_proximity_enable(const bool value){
    static bool prev_val = false;
    bool res = false;
    if(!prev_val && value){
        res = true;
    }
    prev_val = value;
    return res;
}

/* check left proximity sensor (high edge active) */
bool uvlcControLogiclTask::check_r_proximity_enable(const bool value){
    static bool prev_val = false;
    bool res = false;
    if(!prev_val && value){
        res = true;
    }
    prev_val = value;
    return res;
}

/* check wipe forward button (low edge active) */
bool uvlcControLogiclTask::check_wipe_forward(const bool value){
    static bool prev_val = false;
    bool res = false;
    if(prev_val && !value){
        res = true;
    }
    prev_val = value;
    return res;
}

/* check wipe reverse button (low edge active) */
bool uvlcControLogiclTask::check_wipe_reverse(const bool value){
    static bool prev_val = false;
    bool res = false;
    if(prev_val && !value){
        res = true;
    }
    prev_val = value;
    return res;
}

/* check wipe once button (low edge active) */
bool uvlcControLogiclTask::check_wipe_once(const bool value){
    static bool prev_val = false;
    bool res = false;
    if(prev_val && !value){
        res = true;
    }
    prev_val = value;
    return res;
}