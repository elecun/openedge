
#include "pcan.mqtt.task.hpp"
#include <openedge/log.hpp>
#include <openedge/device/bus.hpp>
#include <openedge/device/controller.hpp>
#include <support/device/dkm_dx3000.hpp>
#include <openedge/device/gpio.hpp>
#include <support/device/am335x/gpio.hpp>
#include <string>

#include <errno.h>
#include <arpa/inet.h>		/* inet_ntoa() */
#include <netinet/tcp.h>	/* SOL_TCP... */

#define CLIENT_QUEUE_LEN 	  	3

using namespace oe::support;

//static component instance that has only single instance
static pcanMqttTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new pcanMqttTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool pcanMqttTask::configure(){

    //initialize mosquitto
    mosqpp::lib_init();

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

    //this->connect(_mqtt_host.c_str(), _mqtt_port, _mqtt_keep_alive);
    this->connect_async(_mqtt_host.c_str(), _mqtt_port, _mqtt_keep_alive);
    this->loop_start();

    // //initialize network(UDP)
    // FD_ZERO(&_fds_rd);
	// FD_ZERO(&_fds_wr);

    // //init incoming data socket
    // if(_so_fd_data_in<0){
	// 	_so_fd_data_in = priv_openSocketIn(_dataPort, _protocol_type, CLIENT_QUEUE_LEN);
	// 	if(_so_fd_data_in<0){
    //         console::error("open socket failed({})", strerror(errno));
    //         return false;
	// 	}
	// 	else
    //         console::info("so_fd_data_in is {}", _so_fd_data_in);
	// }else
    //         console::warn("{already open (fd={})}", _so_fd_data_in);

    return true;
}

void pcanMqttTask::execute(){
    const char* msg = "this is test";
    this->publish(nullptr, _mqtt_topic.c_str(), strlen(msg), msg, 2, false);
    
}

void pcanMqttTask::cleanup(){
    this->disconnect();
    this->loop_stop();
    mosqpp::lib_cleanup();
}

void pcanMqttTask::pause(){

}

void pcanMqttTask::resume(){

}


int pcanMqttTask::priv_openSocketIn(int port, int type, unsigned int queue_len)
{
    // struct sockaddr_in sockname;
	// signed long optval = 1;
	// signed long sockID = 0;

	// //New Socket Instance
	// if (type == SOCK_STREAM){
	// 	//New Socket Instance
	// 	if ((sockID = socket(PF_INET, SOCK_STREAM, 0)) < 0){
    //         console::error("Unable to open Socket");
	// 		// exit(-2);			   				/* Rückkehr bei Fehler */
	// 	}
	// }
	// else if (type == SOCK_DGRAM){
	// 	//New Socket Instance
	// 	if ((sockID = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
	// 		console::error("Unable to open Socket");
	// 		// exit(-2);			   				/* Rückkehr bei Fehler */
	// 	}
	// }
	// else
    //     return 0;

	// signed long retVal = setsockopt(sockID, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	// if(retVal < 0){
    //     console::error("error using setsockopt (SOL_SOCKET, SO_REUSEADDR)");
	// 	return 0;
	// }

	// // Enable keepalive packets
	// retVal = setsockopt(sockID, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
	// if(retVal < 0){
    //     console::error("error using setsockopt (SOL_SOCKET, SO_KEEPALIVE)");
	// 	return 0;
	// }

	// retVal = setsockopt(sockID, SOL_IP, IP_RECVERR, &optval, sizeof(int));
	// if(retVal < 0){
    //     console::error("error using setsockopt (SOL_IP, IP_RECVERR)");
	// 	return 0;
	// }

	// //Config Socket
	// memset((char *) &sockname, 0, sizeof(struct sockaddr_in));	/* unbenutzte Bytes loeschen! */
	// sockname.sin_family = AF_INET;	   							/* IP-Adresse aufsetzen */
	// sockname.sin_port = htons(port);							/* Portnummer des Services */
	// sockname.sin_addr.s_addr = htonl(INADDR_ANY);				/* wildcard */


    // if(bind(sockID, (const struct sockaddr*) &sockname, sizeof(sockname)) < 0){	    /* an Adresse binden */
    //     console::error("unalbe to bind socket");
    //     return 0;
    // }

    // if (type == SOCK_STREAM){
    // 	struct sockaddr peer_addr;
    // 	socklen_t addr_len;

	// 	if (listen(sockID, queue_len) < 0){
    //         console::error("unable to listen to clients");
    //         return 0;
	// 	}

	// 	sockID = accept(sockID, &peer_addr, &addr_len);
    // }

	// return sockID;
}

int pcanMqttTask::priv_closeSocket(int sock){
    shutdown(sock, SHUT_RDWR);
	close(sock);
	return 0;
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