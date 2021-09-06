
#include "moxa.io.task.hpp"
#include <openedge/log.hpp>


//static component instance that has only single instance
static moxaIoTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new moxaIoTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool moxaIoTask::configure(){

    json config = json::parse(getProfile()->get("configurations"));

    //read PCAN UDP parameter & open socket
    if(config.find("device")!=config.end()){

        json device = config["device"];
        if(device.find("dataport")!=device.end()) _dataport = device["dataport"].get<int>();
        console::info("> Device Data Port : {}", _dataport);

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
        _sockname.sin_port = htons(_dataport);
        _sockname.sin_addr.s_addr = htonl(INADDR_ANY);

        //socket binding
        if(::bind(_sockfd, (const struct sockaddr*)&_sockname, sizeof(_sockname))<0){
            console::error("unable to bind socket");
            return false;
        }
    }

    return true;
}

void moxaIoTask::execute(){

    const int max_length = 4096;
    unsigned char* buffer = new unsigned char[max_length];
    ::memset(buffer, 0, sizeof(char)*max_length);

    do {
        int len = ::recvfrom(_sockfd, (char*)buffer, max_length, MSG_DONTWAIT, nullptr, nullptr);
        if(len>0){
            console::info("{:x }", buffer);

            ::memset(buffer, 0, sizeof(char)*max_length);
        }
        else
            break;
    }
    while(1);
    
    delete []buffer;

}


void moxaIoTask::cleanup(){

    //UDP connection close
    shutdown(_sockfd, SHUT_RDWR);
	close(_sockfd);
}

void moxaIoTask::pause(){

}

void moxaIoTask::resume(){

}

