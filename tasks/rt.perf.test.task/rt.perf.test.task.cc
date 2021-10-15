
#include "rt.perf.test.task.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static rtPerfTestTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new rtPerfTestTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool rtPerfTestTask::configure(){

    // _gp_manager = GPIOManager::getInstance();

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

    //ping setup
    // auto pin_perf = getPin("P9_12");
    // _gp_manager->exportPin(pin_perf.value().gpio);
    // _gp_manager->setDirection(pin_perf.value().gpio, OUTPUT);

    // auto pin_cycle = getPin("P9_15");
    // _gp_manager->exportPin(pin_cycle.value().gpio);
    // _gp_manager->setDirection(pin_cycle.value().gpio, OUTPUT);
    
    return true;
}

void rtPerfTestTask::execute(){
    
    // rt cycle signal1
    chdir("/sys/class/gpio/gpio48");
    if(_gpio_cycle)
        ::system("echo 0 > value");
    else
        ::system("echo 1 > value");
    _gpio_cycle=!_gpio_cycle;

    //processing
    chdir("/sys/class/gpio/gpio60");
    ::system("echo 1 > value");

        
    
        
}

void rtPerfTestTask::cleanup(){

    //MQTT connection close
    this->disconnect();
    this->loop_stop();
    mosqpp::lib_cleanup();
}

void rtPerfTestTask::pause(){

}

void rtPerfTestTask::resume(){

}

void rtPerfTestTask::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Brocker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);
}

void rtPerfTestTask::on_disconnect(int rc){

}

void rtPerfTestTask::on_publish(int mid){

}

void rtPerfTestTask::on_message(const struct mosquitto_message* message){

    //receive
    const int maxbuffer_size = 4096;
    char* buffer = new char[maxbuffer_size];
    memset(buffer, 0, sizeof(char)*maxbuffer_size);
    memcpy(buffer, message->payload, sizeof(char)*message->payloadlen);
    string strmsg = buffer;
    string topic = message->topic;
    delete []buffer;

    //message topic comparison
    if(!topic.compare("perf/io") && !_io_complete) {
        complete |= 0x01;
        _io_complete = true;
    }
    else if(!topic.compare("perf/ai")&& !_ai_complete) {
        //console::info("perf ai received");
        complete |= 0x02;
        _ai_complete = true;
    }
    else if(!topic.compare("perf/can")&& !_can_complete) {
        //console::info("perf can received");
        complete |= 0x04;
        _can_complete = true;
    }

    if(complete==0x07){
        json pubdata;
        pubdata["test"] = "completed";
        string str_pubdata = pubdata.dump();
        this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(str_pubdata.c_str()), str_pubdata.c_str(), 2, false);   

        chdir("/sys/class/gpio/gpio60");
        ::system("echo 0 > value");
        complete = 0x00;
        _io_complete = false;
        _ai_complete = false;
        _can_complete = false;
    }
    

    // string msg_topic(message->topic);
    // if(!msg_topic.compare("aop/uvlc/sensor")){ //sensor data

    //     try {
    //         json msg = json::parse(strmsg);
    //         if(msg.find("id")!=msg.end()){
    //             int dlc = msg["dlc"].get<int>();
    //             switch(dlc){
    //                 case 4: {
    //                     union {
    //                         unsigned long value;
    //                         float f_value;
    //                     }u;
    //                     vector<unsigned char> valueset = msg["value"];
    //                     u.value = (valueset[3]&0xff)<<24;
    //                     u.value |= (valueset[2]&0xff)<<16;
    //                     u.value |= (valueset[1]&0xff)<<8;
    //                     u.value |= (valueset[0]&0xff);
    //                     string canid = msg["id"].get<string>();
    //                     if(_intensity_id.count(canid)){
    //                         if(_intensity_value.count(_intensity_id[msg["id"].get<string>()])){
    //                             _intensity_value[_intensity_id[msg["id"].get<string>()]] = u.f_value;
    //                         }
    //                     }
    //                     console::info("sensor value : {}({:f})", msg["id"].get<string>(), u.f_value);
    //                 } break;
    //         case 2: {
    //             console::warn("Not support DLC(2) yet.");
    //         } break;
    //         case 1: {
    //             vector<unsigned char> valueset = msg["value"];
    //             _limit_value = valueset[0];
    //             console::info("sensor value : {}(0x{:x})",msg["id"].get<string>(), valueset[0]);
    //         } break;
    //     }  
                
    //         }
    //     }
    //     catch(json::exception& e){
    //         console::error("Message Error : {}", e.what());
    //     }
    // }
}

void rtPerfTestTask::on_subscribe(int mid, int qos_count, const int* granted_qos){
    
}

void rtPerfTestTask::on_unsubscribe(int mid){

}

void rtPerfTestTask::on_log(int level, const char* str){

}

void rtPerfTestTask::on_error(){

}
