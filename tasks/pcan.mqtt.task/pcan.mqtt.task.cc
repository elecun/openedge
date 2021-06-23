
#include "pcan.mqtt.task.hpp"
#include <openedge/log.hpp>
#include <openedge/device/bus.hpp>
#include <openedge/device/controller.hpp>
#include <support/device/dkm_dx3000.hpp>
#include <openedge/device/gpio.hpp>


#define CLIENT_QUEUE_LEN 	  	3

using namespace oe::support;

//static component instance that has only single instance
static pcanMqttTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new pcanMqttTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool pcanMqttTask::configure(){

    //initialize mosquitto
    if(const int ret = mosqpp::lib_init()!=MOSQ_ERR_SUCCESS){
        console::error("({}){}", ret, mosqpp::strerror(ret));
        return false;
    }

    //read configuration from profile
    json config = json::parse(getProfile()->get("configurations"));

    if(config.find("mqtt_host")!=config.end()) _mqtt_host = config["mqtt_host"].get<string>();
    if(config.find("mqtt_port")!=config.end()) _mqtt_port = config["mqtt_port"].get<int>();
    if(config.find("mqtt_topic")!=config.end()) _mqtt_topic = config["mqtt_topic"].get<string>();
    if(config.find("mqtt_keep_alive")!=config.end()) _mqtt_keep_alive = config["mqtt_keep_alive"].get<int>();
    if(config.find("pcan_dataport")!=config.end()) _data_port = config["pcan_dataport"].get<unsigned int>();

    console::info("> set MQTT Host : {}", _mqtt_host);
    console::info("> set MQTT Port : {}", _mqtt_port);
    console::info("> set MQTT Topic : {}", _mqtt_topic);
    console::info("> set PCAN Data Port : {}", _data_port);


    //connect to MQTT broker
    if(const int conret = this->connect_async(_mqtt_host.c_str(), _mqtt_port, _mqtt_keep_alive)==MOSQ_ERR_SUCCESS){
        if(const int ret = this->loop_start()!=MOSQ_ERR_SUCCESS)
            console::warn("({}){}", ret, mosqpp::strerror(ret));
    }
    else
        console::warn("({}){}", conret, mosqpp::strerror(conret)); 

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
	_sockname.sin_port = htons(_data_port);
	_sockname.sin_addr.s_addr = htonl(INADDR_ANY);

    //socket binding
    if(::bind(_sockfd, (const struct sockaddr*)&_sockname, sizeof(_sockname))<0){
        console::error("unable to bind socket");
        return false;
    }

    FD_ZERO(&_fds_rd);
    _psubTask = new std::thread{ &pcanMqttTask::subtask, this };

    return true;
}

void pcanMqttTask::execute(){
    // const char* msg = "this is test";
    // this->publish(nullptr, _mqtt_topic.c_str(), strlen(msg), msg, 2, false);

    const int max_length = 2048;
    unsigned char* buffer = new unsigned char[max_length];
    memset(buffer, 0, sizeof(char)*max_length);
    S_LAN_MSG rec_msg;

    int len = ::recvfrom(_sockfd, (char*)buffer, max_length, 0, nullptr, nullptr);

    if(len>0){
        this->parseDataMsg(buffer, len, &rec_msg);
        this->printData(&rec_msg);
    }

    delete []buffer;
    
}

void pcanMqttTask::cleanup(){
    //UDP connection close
    shutdown(_sockfd, SHUT_RDWR);
	close(_sockfd);

    //MQTT connection close
    this->disconnect();
    this->loop_stop();
    mosqpp::lib_cleanup();
}

void pcanMqttTask::subtask(){
    static bool run = true;
    while(run){
        if(_sockfd>0){
            FD_SET(_sockfd, &_fds_rd);
        }
        FD_SET(STDIN_FILENO, &_fds_rd);

        int fd_sel = select(STDIN_FILENO+1, &_fds_rd, &_fds_wr, nullptr, nullptr); //start waiting
        if(FD_ISSET(STDIN_FILENO, &_fds_rd)){
            
        }
    }

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