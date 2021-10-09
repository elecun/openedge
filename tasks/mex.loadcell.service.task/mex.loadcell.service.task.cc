
#include "mex.loadcell.service.task.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static mexLoadcellServiceTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new mexLoadcellServiceTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool mexLoadcellServiceTask::configure(){

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
            else if(!mqtt_param["method"].get<string>().compare("on_change")) _pub_method = PUBLISH_METHOD::ON_CHANGE;
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

    // Device Port Config
    if(config.find("loadcell")!=config.end()){
        json loadcell_param = config["loadcell"];
        if(loadcell_param.find("port")!=loadcell_param.end()) _port = loadcell_param["port"].get<string>();
        if(loadcell_param.find("baudrate")!=loadcell_param.end()) _baudrate = loadcell_param["baudrate"].get<unsigned int>();
        console::info("> set Device Port : {}", _port);
        console::info("> set Port Baudrate : {}", _baudrate);

        if(!_tty){
            _tty = new SerialPort(_port.c_str(), BaudRate::B_9600);
            _tty->SetTimeout(-1); // Block when reading until any data is received
	        _tty->Open();
        }
            
    }
    
    return true;
}

void mexLoadcellServiceTask::execute(){
    
    
}

void mexLoadcellServiceTask::cleanup(){

    if(_tty){
        _tty->Close();
        delete _tty;
    }
    // json pubdata;
    // pubdata["command"] = "stop";
    // string str_pubdata = pubdata.dump();
    // this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(str_pubdata.c_str()), str_pubdata.c_str(), 2, false);   
    
    //MQTT connection close
    this->disconnect();
    this->loop_stop();
    mosqpp::lib_cleanup();
}

void mexLoadcellServiceTask::pause(){

}

void mexLoadcellServiceTask::resume(){

}

void mexLoadcellServiceTask::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Brocker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);
}

void mexLoadcellServiceTask::on_disconnect(int rc){

}

void mexLoadcellServiceTask::on_publish(int mid){

}

void mexLoadcellServiceTask::on_message(const struct mosquitto_message* message){

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

void mexLoadcellServiceTask::on_subscribe(int mid, int qos_count, const int* granted_qos){
    
}

void mexLoadcellServiceTask::on_unsubscribe(int mid){

}

void mexLoadcellServiceTask::on_log(int level, const char* str){

}

void mexLoadcellServiceTask::on_error(){

}
