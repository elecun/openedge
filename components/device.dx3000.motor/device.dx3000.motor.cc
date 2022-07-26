
#include "device.dx3000.motor.hpp"
#include <openedge/log.hpp>
#include <openedge/device/bus.hpp>
#include <openedge/device/controller.hpp>
#include <support/device/dkm/dx3000.hpp>

//static component instance that has only single instance
static device_dx3000_motor* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new device_dx3000_motor(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

using namespace oe::support;


void device_dx3000_motor::execute(){
 
}

void device_dx3000_motor::stop(){

}

bool device_dx3000_motor::configure(){

    try {

        const json& profile = this->get_profile()->raw();
        if(profile.contains(PROFILE_CONFIGURATIONS_KEY)){
            json config = profile[PROFILE_CONFIGURATIONS_KEY];

            /* device configurations */
            if(!config.contains("device")){
                console::error("Device configurations does not exist");
                return false;
            }
            json device_param = config["device"];
            _gear_ratio = device_param["gear_ratio"].get<double>();
            _default_rpm = device_param["default_rpm"].get<int>();
            _gateway_ip = device_param["gateway_ip"].get<string>();
            _gateway_port = device_param["gateway_port"].get<int>();
            _slave_id = device_param["slave_id"].get<int>();

            console::info("> DX3000 gatway ({}, {})", _gateway_ip, _gateway_port);

            _controller = new oe::support::DKM_DX3000_NATIVE(_slave_id);
            if(!_controller->open()){
                console::error("DX3000 device open error");
                return false;
            }
            else {
                DKM_DX3000_NATIVE* motor = dynamic_cast<DKM_DX3000_NATIVE*>(_controller);
                if(motor){
                    motor->set_parameter(DKM_DX3000_NATIVE::PARAMETER::SET_ID, _slave_id);
                    console::info("> Use DX3000 Controller ({})", _slave_id);

                    //set default rpm
                    unsigned char frame[8] = {0x00,};
                    int size = motor->set_rpm(frame, (unsigned short)_default_rpm);
                    ::sendto(_sockfd, frame, sizeof(frame), 0, (struct sockaddr*)&_sockname, sizeof(_sockname));
                    console::info("> Set Motor default RPM : {}", _default_rpm);
                }
            }

            /* UDP Socket config */
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

            ::memset((char*)&_sockname, 0, sizeof(struct sockaddr_in));
            _sockname.sin_family = AF_INET;
            _sockname.sin_port = htons(_gateway_port);
            _sockname.sin_addr.s_addr = inet_addr(_gateway_ip.c_str());


            /* mqtt configurations */
            if(!config.contains("mqtt")){
                console::error("MQTT configurations does not exist");
                return false;
            }

            json mqtt_param = config["mqtt"];
            _broker_address = mqtt_param["broker"].get<string>();
            _broker_port = mqtt_param["port"].get<int>();
            _pub_topic = mqtt_param["pub_topic"].get<string>();
            _pub_qos = mqtt_param["pub_qos"].get<int>();
            _keep_alive = mqtt_param["keep_alive"].get<int>();
            if(mqtt_param.find("sub_topic")!=mqtt_param.end()){
                for(json::iterator itr=mqtt_param["sub_topic"].begin(); itr!=mqtt_param["sub_topic"].end(); ++itr){
                    _mqtt_sub_topics.emplace_back(*itr);
                }
            }

            console::info("> set MQTT Broker : {}", _broker_address);
            console::info("> set MQTT Port : {}", _broker_port);
            console::info("> set MQTT Pub. Topic : {}", _pub_topic);
            console::info("> set MQTT Pub. QoS : {}", _pub_qos);
            console::info("> set MQTT Keep-alive : {}", _keep_alive);

            //connect to MQTT broker
            if(const int conret = this->connect_async(_broker_address.c_str(), _broker_port, _keep_alive)==MOSQ_ERR_SUCCESS){
                for(string topic:_mqtt_sub_topics){
                    this->subscribe(nullptr, topic.c_str(), 2);
                    console::info("> Subscribe topic : {}", topic);
                }
                    
                if(const int ret = this->loop_start()!=MOSQ_ERR_SUCCESS)
                    console::warn("({}){}", ret, mosqpp::strerror(ret));
            }
            else
                console::warn("({}){}", conret, mosqpp::strerror(conret));

        }
        else
            return false;

    }
    catch(const json::exception& e){
        console::error("Error : {}", e.what());
        return false;
    }

    return true;
}

void device_dx3000_motor::cleanup(){

    /* motor stop */
    DKM_DX3000_NATIVE* motor = dynamic_cast<DKM_DX3000_NATIVE*>(_controller);
    unsigned char frame[8] = {0x00,};
    if(motor){
        int size = motor->stop(frame);
        ::sendto(_sockfd, frame, sizeof(frame), 0, (struct sockaddr*)&_sockname, sizeof(_sockname));
        console::info("motor stop");
    }

    /* UDP socket close */
    shutdown(_sockfd, SHUT_RDWR);
	close(_sockfd);

    /* mqtt connection close */
    this->mosqpp::mosquittopp::disconnect();
    this->mosqpp::mosquittopp::loop_stop();
    mosqpp::lib_cleanup();

}

void device_dx3000_motor::pause(){
    
}

void device_dx3000_motor::resume(){
    
}


void device_dx3000_motor::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Broker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);

}
void device_dx3000_motor::on_disconnect(int rc){

}
void device_dx3000_motor::on_publish(int mid){

}
void device_dx3000_motor::on_message(const struct mosquitto_message* message){

    /* command processing */
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
        if(msg.contains("command")){
            string command = msg["command"].get<string>();
            std::transform(command.begin(), command.end(), command.begin(),[](unsigned char c){ return std::tolower(c); }); //to lower case

            console::info("motor command : {}", command);
            if(_dx_command.count(command)){
                switch(_dx_command[command]){
                    case 1: { //move cw
                        unsigned char frame[8] = {0x00,};
                        int size = motor->move(frame, DKM_DX3000_NATIVE::DIRECTION::CW);
                        ::sendto(_sockfd, frame, sizeof(frame), 0, (struct sockaddr*)&_sockname, sizeof(_sockname));
                        console::info("Motor moves backward(CW)");
                    } break;
                    case 2: { //move ccw
                        unsigned char frame[8] = {0x00,};
                        int size = motor->move(frame, DKM_DX3000_NATIVE::DIRECTION::CCW);
                        ::sendto(_sockfd, frame, sizeof(frame), 0, (struct sockaddr*)&_sockname, sizeof(_sockname));
                        console::info("Motor moves forward(CCW)");
                    } break;
                    case 3: { //stop
                        unsigned char frame[8] = {0x00,};
                        int size = motor->stop(frame);
                        ::sendto(_sockfd, frame, sizeof(frame), 0, (struct sockaddr*)&_sockname, sizeof(_sockname));
                        console::info("Motor stop");
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
void device_dx3000_motor::on_subscribe(int mid, int qos_count, const int* granted_qos){

}
void device_dx3000_motor::on_unsubscribe(int mid){

}
void device_dx3000_motor::on_log(int level, const char* str){

}
void device_dx3000_motor::on_error(){
    
}
