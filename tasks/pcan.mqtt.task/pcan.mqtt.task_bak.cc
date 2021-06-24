
#include "pcan.mqtt.task.hpp"
#include <openedge/log.hpp>
#include <openedge/device/bus.hpp>
#include <openedge/device/controller.hpp>
#include <support/device/dkm_dx3000.hpp>

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
        FD_ZERO(&_fds_rd);
	    FD_ZERO(&_fds_wr);

        json pcan_param = config["pcan"];
        if(pcan_param.find("dataport")!=pcan_param.end()) _pcan_dataport = pcan_param["dataport"].get<int>();

        console::info("> set PCAN Data Port : {}", _pcan_dataport);

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
        memset((char*)&_sockname, 0, sizeof(struct sockaddr_in));
        _sockname.sin_family = AF_INET;
        _sockname.sin_port = htons(_pcan_dataport);
        _sockname.sin_addr.s_addr = htonl(INADDR_ANY);

        //socket binding
        if(::bind(_sockfd, (const struct sockaddr*)&_sockname, sizeof(_sockname))<0){
            console::error("unable to bind socket");
            return false;
        }

        _psubTask = new std::thread{ &pcanMqttTask::subtask, this };
    }

    return true;
}

void pcanMqttTask::execute(){
    // const char* msg = "this is test";
    // this->publish(nullptr, _mqtt_topic.c_str(), strlen(msg), msg, 2, false);

    const int max_length = 2048;
    unsigned char* buffer = new unsigned char[max_length];
    memset(buffer, 0, sizeof(char)*max_length);
    S_LAN_MSG rec_msg;

    // while(1){
    //     int len = ::recvfrom(_sockfd, (char*)buffer, max_length, 0, nullptr, nullptr);
    //     if(len>0){
    //         console::info("receive length : {}", len);
    //         this->parse_pcan_data(buffer, len, &rec_msg);
    //         this->print_pcan_data(&rec_msg);
    //     }
    //     else break;
    //     usleep(1);
    // }

    //limit sensor check

    delete []buffer;
    
}

void pcanMqttTask::subtask(){
    static bool run = true;
    int fdmax = 0;

    while(run){

        if(_sockfd>0)
			FD_SET(_sockfd, &_fds_rd);
		FD_SET(STDIN_FILENO, &_fds_rd);

        fdmax = (STDIN_FILENO>fdmax)?STDIN_FILENO:fdmax;
		fdmax = (_sockfd>fdmax)?_sockfd:fdmax;

        //start waiting
        int fd_sel = select(fdmax+1, &_fds_rd, &_fds_wr, nullptr, nullptr);

        //waiting failed
        if(fd_sel<0){
            console::warn("Waiting failed");
        }
        else if(fd_sel==0){ //time out
            console::warn("Waiting timeout");
            continue;
        }

        //check every bit set in descriptor set
        for (int i=0; i<=fdmax; i++) {
			if(FD_ISSET(i, &_fds_rd)) {
				fd_sel--;
				FD_CLR(i, &_fds_rd);

				if(i==STDIN_FILENO){ //should be first
					handle_loc_cmd(STDIN_FILENO);
				}
                else if (i==_sockfd){
					handle_rem_data(i);
				} else{

				}
			}

			if (FD_ISSET(i, &_fds_wr)){
				fd_sel--;
				FD_CLR(i, &_fds_wr);
			}

			if (!fd_sel)
				break;
		}
    }
}

void pcanMqttTask::cleanup(){
    DKM_DX3000* motor = dynamic_cast<DKM_DX3000*>(_controller);
    if(motor)
        motor->stop();
    _controller->close();

    //UDP connection close
    shutdown(_sockfd, SHUT_RDWR);
	close(_sockfd);

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
    data+= fmt::format("{:x} ", p_msg->dlc);

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


int pcanMqttTask::handle_loc_cmd(int fd){
    char cmdbuf[MAX_LOC_CMD_LEN+1];
	int len=0, i=0;

	if(fd!=STDIN_FILENO) /* not STDIN_FILENO */
		return -1;

	len = read(fd, &cmdbuf[len], MAX_LOC_CMD_LEN-len);

	if (verbose>3){
		printf("got %d chars:\n   ", len);
		for(i=0; i<len; i++)
			printf("%0x2 ", cmdbuf[i]);
	}

	if (len == 0) {
		if (verbose > 1)
			printf("%s(): nothing to read on %d\n", __func__, fd);
		return -2;
	} else if(len == MAX_LOC_CMD_LEN){
		if (verbose > 1)
			printf("%s(): maybe more then %d chars to read on %d\n", __func__, MAX_LOC_CMD_LEN, fd);
		//@ ToDo return here?
		//return;
	}
	cmdbuf[len]=0;

	if (verbose > 3)
		printf("%s(): cmd line=%s\n", __func__, cmdbuf);

	i=0;
	while(loc_cmds[i].cmd_text){
		if(strstr(cmdbuf, loc_cmds[i].cmd_text))
			break;
		else
			i++;
	}

	if(!loc_cmds[i].cmd_text){
		if (verbose > 2)
				printf("%s(): no such command found: %s\n", __func__, cmdbuf);
		return -3;
	}

	if(!loc_cmds[i].cmd_handler){
		if (verbose > 2)
				printf("%s(): no handler defined for command: %s\n", __func__, cmdbuf);
		return -4;
	}

	return loc_cmds[i].cmd_handler(&cmdbuf[strlen(loc_cmds[i].cmd_text)]);
}

int pcanMqttTask::handle_rem_data(int fd){

    static unsigned char buff[BUFF_SZ];
	unsigned int flgs=0;
	unsigned int cur_pos=0;
	static unsigned int remaining_data=0;
	int res=0, i=0, rec_len;
	S_LAN_MSG rec_msg;

	::memset(&buff[remaining_data], 0,BUFF_SZ-remaining_data);
	rec_len = recv(fd , &buff[remaining_data], BUFF_SZ-remaining_data, flgs );
	rec_len += remaining_data;
	remaining_data=0;

	if (rec_len < 0){
		if(verbose)
			printf("%s(): error while receiving data (%s)\n",
					__func__, strerror(errno));
	} else if(rec_len ==0){
		if(verbose)
			printf("socket %d: remote disconnection\n", fd);
		priv_closeSocket(fd);
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

			if(verbose>1)
				printData(&rec_msg);
		}
	}

	return 0;
}