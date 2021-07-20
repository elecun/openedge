
#include "pcan.mqtt.task.hpp"
#include <openedge/log.hpp>


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

        if(pcan_param.find("acceptable")!=pcan_param.end()){
            json node = pcan_param["acceptable"];
            for(json::iterator itr = node.begin(); itr!=node.end(); ++itr){
                _pcan_node[*itr] = new PCANNode();
                console::info("> Acceptable CAN ID : {}", *itr);
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

    const int max_length = 4096;
    unsigned char* buffer = new unsigned char[max_length];
    ::memset(buffer, 0, sizeof(char)*max_length);

    do {
        int len = ::recvfrom(_sockfd, (char*)buffer, max_length, MSG_DONTWAIT, nullptr, nullptr);
        if(len>0){
            S_LAN_MSG rec_msg;
            this->parse_pcan_data(buffer, len, &rec_msg);
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

    json can_data;
    can_data["id"] = fmt::format("0x{:x}", p_msg->id);
    can_data["dlc"] = p_msg->dlc;
    can_data["timestamp"] = p_msg->timestamp;
    can_data["msgtype"] = fmt::format("0x{:x}", p_msg->type);
    can_data["flag"] = p_msg->flag;
    can_data["size"] = p_msg->size;
    can_data["tag"] = p_msg->tag;

    vector<uint8_t> valset;
    for(unsigned char v:p_msg->value.Value8u){
        valset.emplace_back(v);
    }
    can_data["value"] = valset;

    string msg = can_data.dump();
    this->publish(nullptr, _mqtt_pub_topic.c_str(), strlen(msg.c_str()), msg.c_str(), 2, false); //data publish
    console::info(msg);
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