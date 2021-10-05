
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
            console::info("> set UVLC Limit Sensor CAN ID : 0x{:x}", _limit_id);
        }

        if(uvlc_param.find("intensity_sensor")!=uvlc_param.end()){
            for(json::iterator itr=uvlc_param["intensity_sensor"].begin(); itr!=uvlc_param["intensity_sensor"].end(); ++itr){
                string value = itr->get<string>();
                _intensity_id[value] = (unsigned short)std::stoul(value.c_str(), nullptr, 16);
                console::info("> set UVLC Intensity Sensor CAN ID : 0x{:x}", _intensity_id[value]);
            }
        }

        if(uvlc_param.find("intensity_threshold")!=uvlc_param.end()) { _intensity_threshold = uvlc_param["intensity_threshold"].get<float>(); }
        console::info("> set UVLC Intensity Threshold : {}", _intensity_threshold);

    }
    
    return true;
}

void uvlcControlTask::execute(){
    static LIMIT_STATE _limit_state = LIMIT_STATE::NO_LIMIT_ACTIVE;
    static UVLC_WORK_STATE _uvlc_state = UVLC_WORK_STATE::READY;

    if(!(_limit_value&0x0c) && _limit_state!=LIMIT_STATE::NO_LIMIT_ACTIVE){

    }
    else if(!(_limit_value&0x04) && _limit_state!=LIMIT_STATE::L_LIMIT_ACTIVE){
        json cmd;
        cmd["command"] = "move_cw";
        string msg = cmd.dump();
        this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(msg.c_str()), msg.c_str(), 2, false); //data publish
        _limit_state = LIMIT_STATE::L_LIMIT_ACTIVE;
    }
    else if(!(_limit_value&0x08) && _limit_state!=LIMIT_STATE::R_LIMIT_ACTIVE){
        json cmd;
        cmd["command"] = "move_cw";
        string msg = cmd.dump();
        this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(msg.c_str()), msg.c_str(), 2, false); //data publish
        _limit_state = LIMIT_STATE::R_LIMIT_ACTIVE;
    }
    else
        console::warn("Undefined UVLC Sensor State : 0x{:x}", _limit_value);
    

    float avg = 0.0;
    for(map<string, unsigned short>::iterator itr = _intensity_id.begin(); itr!=_intensity_id.end(); ++itr){
        avg += _intensity_value[itr->second];
    }
    avg = avg/_intensity_value.size();
    if(avg<_intensity_threshold && _uvlc_state!=UVLC_WORK_STATE::WORK){
        json cmd;
        cmd["command"] = "move_cw";
        string msg = cmd.dump();
        this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(msg.c_str()), msg.c_str(), 2, false); //data publish
        _uvlc_state=UVLC_WORK_STATE::WORK;
    }
    else {
        _uvlc_state=UVLC_WORK_STATE::READY;
    }
    
}

void uvlcControlTask::cleanup(){
    json cmd;
    cmd["command"] = "stop";
    string msg = cmd.dump();
    this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(msg.c_str()), msg.c_str(), 2, false); //data publish

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
                int dlc = msg["dlc"].get<int>();
                switch(dlc){
                    case 4: {
                        union {
                            unsigned long value;
                            float f_value;
                        }u;
                        vector<unsigned char> valueset = msg["value"];
                        u.value = (valueset[3]&0xff)<<24;
                        u.value |= (valueset[2]&0xff)<<16;
                        u.value |= (valueset[1]&0xff)<<8;
                        u.value |= (valueset[0]&0xff);
                        string canid = msg["id"].get<string>();
                        if(_intensity_id.count(canid)){
                            if(_intensity_value.count(_intensity_id[msg["id"].get<string>()])){
                                _intensity_value[_intensity_id[msg["id"].get<string>()]] = u.f_value;
                            }
                        }
                        console::info("sensor value : {}({:f})", msg["id"].get<string>(), u.f_value);
                    } break;
            case 2: {
                console::warn("Not support DLC(2) yet.");
            } break;
            case 1: {
                vector<unsigned char> valueset = msg["value"];
                _limit_value = valueset[0];
                console::info("sensor value : {}(0x{:x})",msg["id"].get<string>(), valueset[0]);
            } break;
        }  
                
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
