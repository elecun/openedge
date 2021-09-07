
#include "moxa.io.task.hpp"
#include <openedge/log.hpp>
#include <3rdparty/libmodbus/modbus.h>


//static component instance that has only single instance
static moxaIoTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new moxaIoTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

//static instance
static modbus_t* _modbus = nullptr;

bool moxaIoTask::configure(){

    //1. read configurations
    json config = json::parse(getProfile()->get("configurations"));

    //1.1 network configuration
    if(config.find("network")!=config.end()){

        json network = config["network"];
        if(network.find("dataport")!=network.end()) _dataport = network["dataport"].get<int>();
        console::info("> Data Port(UDP) : {}", _dataport);

        //start UDP Server
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
        _sockname.sin_port = htons(_dataport);
        _sockname.sin_addr.s_addr = htonl(INADDR_ANY);

        //socket binding
        if(::bind(_sockfd, (const struct sockaddr*)&_sockname, sizeof(_sockname))<0){
            console::error("unable to bind socket");
            return false;
        }
    }

    // 2. Device configuration
    if(config.find("device")!=config.end()){
        json device = config["device"];
        _devicename = device["name"].get<string>();
        _deviceip = device["ip"].get<string>();
        console::info("Use device {}({})", _devicename, _deviceip);

        //2.1 DI Config
        if(device.find("di")!=device.end()){
            json _di = device["di"];
            for(json::iterator itr=_di.begin(); itr!=_di.end(); ++itr){
                console::info("Config DI : {}({})", (*itr)["name"].get<std::string>(), (*itr)["pin"].get<int>());
                di_container.insert(make_pair((*itr)["name"].get<std::string>(), (*itr)["pin"].get<int>()));
            }
        }

        //2.2 DO Config
        if(device.find("do")!=device.end()){
            json _do = device["do"];
            for(json::iterator itr=_do.begin(); itr!=_do.end(); ++itr){
                console::info("Config DO : {}({})", (*itr)["name"].get<std::string>(), (*itr)["pin"].get<int>());
                do_container.insert(make_pair((*itr)["name"].get<std::string>(), (*itr)["pin"].get<int>()));
            }
        }
    }

    //3. config modbus
    if(!_modbus){
        _modbus = modbus_new_tcp(_deviceip.c_str(), 502);
        if(!_modbus){
            console::error("Unable to create the modbus context");
            return false;
        }

        if(modbus_set_slave(_modbus, 1) == -1){
            console::warn("Modbus Set slave failed");
        }

        if(modbus_connect(_modbus)==-1){
            console::error("Device connection failed : {}", modbus_strerror(errno));
            modbus_free(_modbus);
            return false;
        }
    }

    return true;
}

void moxaIoTask::execute(){

    if(_modbus){
        unsigned short di_value = 0x0000;
        if(modbus_read_input_registers(_modbus, 30048, 2, &di_value))
            console::info("0x{0:x}", di_value);
    }

    // const int max_length = 4096;
    // unsigned char* buffer = new unsigned char[max_length];
    // ::memset(buffer, 0, sizeof(char)*max_length);

    // do {
    //     int len = ::recvfrom(_sockfd, (char*)buffer, max_length, MSG_DONTWAIT, nullptr, nullptr);
    //     if(len>0){
    //         console::info("{:x }", buffer);

    //         ::memset(buffer, 0, sizeof(char)*max_length);
    //     }
    //     else
    //         break;
    // }
    // while(1);
    
    // delete []buffer;

}


void moxaIoTask::cleanup(){

    if(_modbus){
        modbus_close(_modbus);
        modbus_free(_modbus);
    }

    //UDP connection close
    shutdown(_sockfd, SHUT_RDWR);
	close(_sockfd);
}

void moxaIoTask::pause(){

}

void moxaIoTask::resume(){

}

