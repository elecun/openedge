
#include "dx3000.motor.service.task.hpp"
#include <openedge/log.hpp>
#include <openedge/device/bus.hpp>
#include <openedge/device/controller.hpp>
#include <support/device/dkm_dx3000.hpp>

using namespace oe::support;

//static component instance that has only single instance
static dx3000MotorServiceTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new dx3000MotorServiceTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool dx3000MotorServiceTask::configure(){

    //clear controller instance if exist
    if(_controller){
        _controller->close();
        delete _controller;
        _controller = nullptr;
    }

    //1. read configuration from profile
    json config = json::parse(getProfile()->get("configurations"));

    //read dx3000 parameters and create instance
    if(config.find("dx3000")!=config.end()){
        json dx3000_param = config["dx3000"];
        if(dx3000_param.find("port")!=dx3000_param.end()){
            _port = dx3000_param["port"].get<int>();
            _gateway = dx3000_param["gateway"].get<string>();
            int slave_id = dx3000_param["slave_id"].get<int>();
            int default_rpm = dx3000_param["default_rpm"].get<int>();

            console::info("> DX3000 gatway ({}, {})", _gateway, _port);
            
            _controller = new oe::support::DKM_DX3000_NATIVE(slave_id);

            if(!_controller->open())
                return false;
            else {
                DKM_DX3000_NATIVE* motor = dynamic_cast<DKM_DX3000_NATIVE*>(_controller);
                if(motor){
                    motor->set_parameter(DKM_DX3000_NATIVE::PARAMETER::SET_ID, slave_id);
                    console::info("> Use DX3000 Controller ({})", slave_id);
                }
            }

            //start UDP socket
            if((_sockfd = ::socket(PF_INET, SOCK_DGRAM, 0))<0){
                console::error("UDP Socket creation failed");
                return false;
            }

            //setting socket options
            if(setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &_sock_optval, sizeof(_sock_optval))<0){
                console::warn("socket error using setsockopt (SOL_SOCKET, SO_REUSEADDR");
            }
            if(setsockopt(_sockfd, SOL_IP, IP_RECVERR, &_sock_optval, sizeof(int))<0){
                console::warn("socket error using setsockopt (SOL_IP, IP_RECVERR)");
            }

            //config socket
            ::memset((char*)&_sockname, 0, sizeof(struct sockaddr_in));
            _sockname.sin_family = AF_INET;
            _sockname.sin_port = htons(_port);
            //_sockname.sin_addr.s_addr = htonl(INADDR_ANY);
            _sockname.sin_addr.s_addr = inet_addr(_gateway.c_str());

            // //socket binding (for server)
            // if(::bind(_sockfd, (const struct sockaddr*)&_sockname, sizeof(_sockname))<0){
            //     console::error("unable to bind socket");
            //     return false;
            // }
        }
    }
    else {
        console::error("cannot read configurations of DX3000 in profile");
        return false;
    }

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

    return true;
}

void dx3000MotorServiceTask::execute(){
    // DKM_DX3000* motor = dynamic_cast<DKM_DX3000*>(_controller);
    // if(motor){
    //     json pubmsg;
    //     pubmsg["rpm"] = motor->get_rpm();
    //     pubmsg["status"] = motor->get_status();
    //     string str_pubmsg = pubmsg.dump();
    //     this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(str_pubmsg.c_str()), str_pubmsg.c_str(), 2, false);
    //     console::info("publish : {}", str_pubmsg);
    // }

}

void dx3000MotorServiceTask::cleanup(){

    // DKM_DX3000_NATIVE* motor = dynamic_cast<DKM_DX3000_NATIVE*>(_controller);
    // if(motor)
    //     motor->stop();
    // _controller->close();

    shutdown(_sockfd, SHUT_RDWR);
	close(_sockfd);

    //MQTT connection close
    this->disconnect();
    this->loop_stop();
    mosqpp::lib_cleanup();
    
}

void dx3000MotorServiceTask::pause(){

}

void dx3000MotorServiceTask::resume(){

}

void dx3000MotorServiceTask::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Brocker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);
}

void dx3000MotorServiceTask::on_disconnect(int rc){

}

void dx3000MotorServiceTask::on_publish(int mid){

}

void dx3000MotorServiceTask::on_message(const struct mosquitto_message* message){
    #define MAX_BUFFER_SIZE     1024

    // read buffer
    char* buffer = new char[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(char)*MAX_BUFFER_SIZE);
    memcpy(buffer, message->payload, sizeof(char)*message->payloadlen);
    string topic(message->topic);
    string strmsg = buffer;
    delete []buffer;

    try{
        json msg = json::parse(strmsg);
        DKM_DX3000_NATIVE* motor = dynamic_cast<DKM_DX3000_NATIVE*>(_controller);

        //command
        if(msg.find("command")!=msg.end()){
            string command = msg["command"].get<string>();
            std::transform(command.begin(), command.end(), command.begin(),[](unsigned char c){ return std::tolower(c); }); //to lower case

            if(_dx_command.count(command)){
                switch(_dx_command[command]){
                    case 1: { //move cw
                        unsigned char frame[8] = {0x00,};
                        int size = motor->move(frame, DKM_DX3000_NATIVE::DIRECTION::CW);
                        ::sendto(_sockfd, frame, sizeof(frame), 0, (struct sockaddr*)&_sockname, sizeof(_sockname));
                        console::info("Motor moves forward(CW)");
                    } break;
                    case 2: { //move ccw
                        unsigned char frame[8] = {0x00,};
                        int size = motor->move(frame, DKM_DX3000_NATIVE::DIRECTION::CCW);
                        ::sendto(_sockfd, frame, sizeof(frame), 0, (struct sockaddr*)&_sockname, sizeof(_sockname));
                        console::info("Motor moves backward(CCW)");
                    } break;
                    case 3: { //stop
                        unsigned char frame[8] = {0x00,};
                        int size = motor->stop(frame);
                        ::sendto(_sockfd, frame, sizeof(frame), 0, (struct sockaddr*)&_sockname, sizeof(_sockname));
                    } break;
                    case 4: { //set rpm
                        unsigned char frame[8] = {0x00,};
                        int value = msg["value"].get<int>();
                        int size = motor->set_rpm(frame, (unsigned short)value);
                        ::sendto(_sockfd, frame, sizeof(frame), 0, (struct sockaddr*)&_sockname, sizeof(_sockname));
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

void dx3000MotorServiceTask::on_subscribe(int mid, int qos_count, const int* granted_qos){
    
}

void dx3000MotorServiceTask::on_unsubscribe(int mid){

}

void dx3000MotorServiceTask::on_log(int level, const char* str){

}

void dx3000MotorServiceTask::on_error(){

}
