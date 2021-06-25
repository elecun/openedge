
#include "pcan.mqtt.task.hpp"
#include <openedge/log.hpp>
#include <openedge/device/bus.hpp>
#include <openedge/device/controller.hpp>
#include <support/device/dkm_dx3000.hpp>
#include "uvlc.control.hpp"

using namespace oe::support;

//static component instance that has only single instance
static pcanMqttTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new pcanMqttTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool pcanMqttTask::configure(){

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

    //read PCAN UDP parameter & open socket
    if(config.find("pcan")!=config.end()){

        json pcan_param = config["pcan"];
        if(pcan_param.find("dataport")!=pcan_param.end()) _pcan_dataport = pcan_param["dataport"].get<int>();
        console::info("> set PCAN Data Port : {}", _pcan_dataport);

        if(pcan_param.find("node")!=pcan_param.end()){
            json node = pcan_param["node"];
            for(json::iterator itr = node.begin(); itr!=node.end(); ++itr){
                _pcan_node[*itr] = new PCANNode();
                console::info("> Add CAN node : {}", *itr);
            }
        }

        //start UDP Server
        if((_sockfd = ::socket(PF_INET, SOCK_DGRAM, 0))<0){
            console::error("PCAN UDP Socket creation failed");
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
        _sockname.sin_port = htons(_pcan_dataport);
        _sockname.sin_addr.s_addr = htonl(INADDR_ANY);

        //socket binding
        if(::bind(_sockfd, (const struct sockaddr*)&_sockname, sizeof(_sockname))<0){
            console::error("unable to bind socket");
            return false;
        }
    }

    return true;
}

void pcanMqttTask::execute(){
    const char* msg = "this is test";
    this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(msg), msg, 2, false);

    const int max_length = 2048;
    unsigned char* buffer = new unsigned char[max_length];
    ::memset(buffer, 0, sizeof(char)*max_length);

    do {
        int len = ::recvfrom(_sockfd, (char*)buffer, max_length, MSG_DONTWAIT, nullptr, nullptr);
        if(len>0){
            S_LAN_MSG rec_msg;
            this->parse_pcan_data(buffer, len, &rec_msg);
            this->print_pcan_data(&rec_msg);
            this->process_pcan_data(&rec_msg);

            ::memset(buffer, 0, sizeof(char)*max_length);
        }
        else
            break;
    }
    while(1);
    
    delete []buffer;

}


void pcanMqttTask::cleanup(){
    DKM_DX3000* motor = dynamic_cast<DKM_DX3000*>(_controller);
    if(motor)
        motor->stop();
    _controller->close();

    //UDP connection close
    shutdown(_sockfd, SHUT_RDWR);
	close(_sockfd);

    //clear memory
    for(map<string, PCANNode*>::iterator itr = _pcan_node.begin(); itr!=_pcan_node.end(); ++itr){
        delete itr->second;
    }
    _pcan_node.clear();

    //MQTT connection close
    this->disconnect();
    this->loop_stop();
    mosqpp::lib_cleanup();
}

void pcanMqttTask::pause(){

}

void pcanMqttTask::resume(){

}


void pcanMqttTask::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Brocker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);
}

void pcanMqttTask::on_disconnect(int rc){

}

void pcanMqttTask::on_publish(int mid){

}

void pcanMqttTask::on_message(const struct mosquitto_message* message){
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

            //move command
            if(!command.compare("move")){
                string direction = msg["direction"].get<string>();
                std::transform(direction.begin(), direction.end(), direction.begin(),[](unsigned char c){ return std::tolower(c); });

                if(!direction.compare("cw")){
                    this->setmode(CONTROLMODE::MANUAL);
                    motor->move(DKM_DX3000::DIRECTION::CW);
                }
                else if(!direction.compare("ccw")){
                    this->setmode(CONTROLMODE::MANUAL);
                    motor->move(DKM_DX3000::DIRECTION::CCW);
                }
            }
            //stop command
            else if(!command.compare("stop")){
                this->setmode(CONTROLMODE::MANUAL);
                motor->stop();
            }
            else if(!command.compare("mode")){
                
            }
            else if(!command.compare("rpm")){
                int value = msg["value"].get<int>();
                motor->set_rpm((unsigned short)value);
                
            }
            else {
                console::warn("undefined command : {}", command);
            }
        }
    }
    catch(json::exception& e){
        console::error("Message Error : {}", e.what());
    }
    console::info("mqtt data({}) : {}",message->payloadlen, strmsg);
}

void pcanMqttTask::on_subscribe(int mid, int qos_count, const int* granted_qos){
    
}

void pcanMqttTask::on_unsubscribe(int mid){

}

void pcanMqttTask::on_log(int level, const char* str){

}

void pcanMqttTask::on_error(){

}

void pcanMqttTask::process_pcan_data(S_LAN_MSG* p_msg){
    string str_id = fmt::format("0x{:x}", p_msg->id);
    
    if(_pcan_node.find(str_id)!=_pcan_node.end()){
        _pcan_node[str_id]->set(p_msg);

        int dlc = (int)_pcan_node[str_id]->get()->dlc;
        switch(dlc){
            case 4: {
                union {
                    unsigned long value;
                    float f_value;
                }u;
                u.value = (_pcan_node[str_id]->get()->value.Value8u[3]&0xff)<<24;
                u.value |= (_pcan_node[str_id]->get()->value.Value8u[2]&0xff)<<16;
                u.value |= (_pcan_node[str_id]->get()->value.Value8u[1]&0xff)<<8;
                u.value |= (_pcan_node[str_id]->get()->value.Value8u[0]&0xff);
                console::info("value : {:f}", u.f_value);
            } break;
            case 2: {

            } break;
            case 1: {
                DKM_DX3000* motor = dynamic_cast<DKM_DX3000*>(_controller);
                //sensor is detected
                if(!(_pcan_node[str_id]->get()->value.Value8u[0]&0x04)){
                    motor->stop();
                    console::info("motor stop by sensor");
                }
                if(!(_pcan_node[str_id]->get()->value.Value8u[0]&0x08)){
                    motor->stop();
                    console::info("motor stop by sensor");
                }

            } break;
        }        
    }
    else {
        console::info("cannot found id : {}", str_id);
    }
}

int pcanMqttTask::parse_pcan_data(unsigned char * p_buff, int len, S_LAN_MSG *p_msg){
    /* check message length */
	p_msg->size = ntohs(*((unsigned short*)&p_buff[0]));
	if(p_msg->size != 36){
        console::warn("wrong message length");
		return -1;
	}
	if(p_msg->size > len){
			console::warn("wrong buffer length. message maybe incomplete!");
		return -2;
	}

	/* check message type */
	p_msg->type = ntohs(*((unsigned short*)&p_buff[2]));
	if(p_msg->type != 0x80){
			console::warn("wrong message type");
		return -1;
	}

	/* get message tag */
	p_msg->tag =  ntohl(*((unsigned long*)&p_buff[4]));
	p_msg->tag <<= 32;
	p_msg->tag += ntohl(*((unsigned long*)&p_buff[8]));

	/* get message time stamp */
	p_msg->timestamp += ntohl(*((unsigned long*)&p_buff[16]));
	p_msg->timestamp <<= 32;
	p_msg->timestamp =  ntohl(*((unsigned long*)&p_buff[12]));

	/* get message channel */
	p_msg->channel = p_buff[20];

	/* get message dlc */
	p_msg->dlc = p_buff[21];
	if(p_msg->dlc > 8){
			console::warn("wrong dlc");
		return -1;
	}

	/* get message dlc */
	p_msg->flag = ntohs(*((unsigned short*)&p_buff[22]));

	/* get message time stamp */
	p_msg->id =  ntohl(*((unsigned long*)&p_buff[24]));

	/* get value and set unused bytes to 0 */
	memset(&(p_msg->value.Value8u[0]), 0, 8);
	memcpy(&(p_msg->value.Value8u[0]), &p_buff[28], p_msg->dlc);

	return p_msg->size;
}

int pcanMqttTask::print_pcan_data(S_LAN_MSG* p_msg){
    unsigned int i;

    string data;

    data = fmt::format("{:d}", p_msg->timestamp);
    data+= fmt::format("({:x}) ", p_msg->dlc);

	if(p_msg->id & 0xE0000000)
        data+=fmt::format("0x{:x}",(unsigned int) p_msg->id);
	else
        data+=fmt::format("0x{:x}",(unsigned int) p_msg->id);

	if(p_msg->dlc)
        data+=" : ";

	for(i=0; i<p_msg->dlc; i++)
        data+=fmt::format("0x{:x} ", p_msg->value.Value8u[i]);

	console::info(data);

	return 0;
}

void pcanMqttTask::setmode(CONTROLMODE mode){
    static CONTROLMODE prev_mode = CONTROLMODE::AUTOMATIC;
    if(prev_mode!=mode){
        _control_mode = mode;
        switch(mode){
            case CONTROLMODE::AUTOMATIC: console::info("Changed control mode : AUTOMATIC"); break;
            case CONTROLMODE::MANUAL: console::info("Changed control mode : MANUAL"); break;
        }
        prev_mode = mode;
    }
}



int pcanMqttTask::handle_rem_data(int fd){

    static unsigned char buff[BUFF_SZ];
	unsigned int flgs=0;
	unsigned int cur_pos=0;
	static unsigned int remaining_data=0;
	int res=0, i=0, rec_len;
	S_LAN_MSG rec_msg;

	memset(&buff[remaining_data], 0,BUFF_SZ-remaining_data);
	rec_len = recv(fd , &buff[remaining_data], BUFF_SZ-remaining_data, flgs );
	rec_len += remaining_data;
	remaining_data=0;

	if (rec_len < 0){
		console::info("{}(): error while receiving data ({})\n", __func__, strerror(errno));
	} 
    else if(rec_len ==0){
		console::info("socket {}: remote disconnection", fd);
		//priv_closeSocket(fd);
		return -2;
	} 
    else {

		while(rec_len>(cur_pos+1)){
			// parse Data until all CAN frames have been extracted
			res=parse_pcan_data(&buff[cur_pos], rec_len-cur_pos, &rec_msg);
			if(res == -2){
				unsigned char tmp_buff[BUFF_SZ];
				remaining_data = rec_len-cur_pos;
				memcpy(tmp_buff, &buff[cur_pos], remaining_data);
				memset(buff, 0, BUFF_SZ);
				memcpy(buff, tmp_buff, remaining_data);
				rec_len=0;
				continue;
			}
			else if(res<0){
				// error during parsing of the message -> quit loop
				rec_len=0;
				continue;
			}
			cur_pos+=res;

			print_pcan_data(&rec_msg);
		}
	}

	return 0;
}