
#include "dx3000.control.task.hpp"
#include <openedge/log.hpp>
#include <openedge/device/bus.hpp>
#include <openedge/device/controller.hpp>
#include <support/device/dkm_dx3000.hpp>

using namespace oe::support;

//static component instance that has only single instance
static dx3000ControlTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new dx3000ControlTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool dx3000ControlTask::configure(){

    //clear controller instance
    if(_controller){
        _controller->close();
        delete _controller;
        _controller = nullptr;
    }

    //initialize mosquitto
    if(const int ret = mosqpp::lib_init()!=MOSQ_ERR_SUCCESS){
        console::error("({}){}", ret, mosqpp::strerror(ret));
        return false;
    }

    //read configuration from profile
    json config = json::parse(getProfile()->get("configurations"));

    //read dx3000 parameters and create instance
    if(config.find("dx3000")!=config.end()){
        json dx3000_param = config["dx3000"];
        if(dx3000_param.find("access")!=dx3000_param.end()){
            string access_port = dx3000_param["access"].get<string>();
            int slave_id = dx3000_param["slave_id"].get<int>();
            int rpm = dx3000_param["rpm"].get<int>();

            _controller = new oe::support::DKM_DX3000(slave_id, access_port.c_str());

            if(!_controller->open())
                return false;
            else {
                DKM_DX3000* motor = dynamic_cast<DKM_DX3000*>(_controller);
                if(motor){
                    motor->set_parameter(DKM_DX3000::PARAMETER::SET_ID, slave_id);
                    motor->set_rpm(rpm);
                    console::info("> Use DX3000 Controller ({}, {})", access_port, slave_id);
                }
            }
        }
    }
    else {
        console::error("cannot read configurations of DX3000 in profile");
        return false;
    }

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

    return true;
}

void dx3000ControlTask::execute(){
    DKM_DX3000* motor = dynamic_cast<DKM_DX3000*>(_controller);
    if(motor){
        json pubmsg;
        pubmsg["rpm"] = motor->get_rpm();
        pubmsg["status"] = motor->get_status();
        string str_pubmsg = pubmsg.dump();
        this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(str_pubmsg.c_str()), str_pubmsg.c_str(), 2, false);
        console::info("publish : {}", str_pubmsg);
    }

}

void dx3000ControlTask::cleanup(){

    DKM_DX3000* motor = dynamic_cast<DKM_DX3000*>(_controller);
    if(motor)
        motor->stop();
    _controller->close();

    //MQTT connection close
    this->disconnect();
    this->loop_stop();
    mosqpp::lib_cleanup();
    
}

void dx3000ControlTask::pause(){

}

void dx3000ControlTask::resume(){

}

void dx3000ControlTask::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Brocker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);
}

void dx3000ControlTask::on_disconnect(int rc){

}

void dx3000ControlTask::on_publish(int mid){

}

void dx3000ControlTask::on_message(const struct mosquitto_message* message){
    #define MAX_BUFFER_SIZE     4096

    char* buffer = new char[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(char)*MAX_BUFFER_SIZE);
    memcpy(buffer, message->payload, sizeof(char)*message->payloadlen);
    string strmsg = buffer;
    delete []buffer;

    try{
        json msg = json::parse(strmsg);
        DKM_DX3000* motor = dynamic_cast<DKM_DX3000*>(_controller);
        if(!motor)
            return;

        //command
        if(msg.find("command")!=msg.end()){
            string command = msg["command"].get<string>();
            std::transform(command.begin(), command.end(), command.begin(),[](unsigned char c){ return std::tolower(c); }); //to lower case

            if(_dx_command.count(command)){
                switch(_dx_command[command]){
                    case 1: { //move cw
                        motor->move(DKM_DX3000::DIRECTION::CW);
                        console::info("Motor moves forward(CW)");
                    } break;
                    case 2: { //move ccw
                        motor->move(DKM_DX3000::DIRECTION::CCW);
                        console::info("Motor moves backward(CCW)");
                    } break;
                    case 3: { //stop
                        motor->stop();
                    } break;
                    case 4: { //rpm
                        int value = msg["value"].get<int>();
                        motor->set_rpm((unsigned short)value);
                        console::info("motor rpm set : {}", value);
                    } break;
                }
            }
        }
    }
    catch(json::exception& e){
        console::error("Message Error : {}", e.what());
    }
    console::info("mqtt data({}) : {}",message->payloadlen, strmsg);
}

void dx3000ControlTask::on_subscribe(int mid, int qos_count, const int* granted_qos){
    
}

void dx3000ControlTask::on_unsubscribe(int mid){

}

void dx3000ControlTask::on_log(int level, const char* str){

}

void dx3000ControlTask::on_error(){

}
