
#include "moxa.analog.service.task.hpp"
#include <openedge/log.hpp>
#include <3rdparty/libmodbus/modbus.h>


//static component instance that has only single instance
static moxaIoServiceTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new moxaIoServiceTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

//static instance
static modbus_t* _modbus = nullptr;

bool moxaIoServiceTask::configure(){

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
                _di_container.insert(make_pair((*itr)["pin"].get<int>(), (*itr)["name"].get<std::string>()));
                _di_values.insert(make_pair((*itr)["pin"].get<int>(), false));
            }
        }

        //2.2 DO Config
        if(device.find("do")!=device.end()){
            json _do = device["do"];
            for(json::iterator itr=_do.begin(); itr!=_do.end(); ++itr){
                console::info("Config DO : {}({})", (*itr)["name"].get<std::string>(), (*itr)["pin"].get<int>());
                _do_container.insert(make_pair((*itr)["pin"].get<int>(), (*itr)["name"].get<std::string>()));
                _do_values.insert(make_pair((*itr)["pin"].get<int>(), false));
            }
        }

        //2.3 modbus config
        if(device.find("modbus_tcp")!=device.end()){
            json modbus_tcp = device["modbus_tcp"];
            if(modbus_tcp.find("port")!=modbus_tcp.end()){
                _modbus_port = modbus_tcp["port"].get<int>();
                console::info("> Modbus TCP Port : {}", _modbus_port);
            }
            if(modbus_tcp.find("di_address")!=modbus_tcp.end()){
                _di_address = modbus_tcp["di_address"].get<int>();
                console::info("> Modbus DI Address : {}", _di_address);
            }
                
            if(modbus_tcp.find("do_address")!=modbus_tcp.end()){
                _do_address = modbus_tcp["do_address"].get<int>();
                console::info("> Modbus DO Address : {}", _do_address);
            }

            // modbus connection
            if(!_modbus){
                _modbus = modbus_new_tcp(_deviceip.c_str(), _modbus_port);
                if(!_modbus){
                    console::error("Unable to create the modbus context");
                    return false;
                }

                if(modbus_connect(_modbus)==-1){
                    console::error("Device connection failed : {}", modbus_strerror(errno));
                    modbus_free(_modbus);
                    return false;
                }
            }
        }
    }

    return true;
}

void moxaIoServiceTask::execute(){

    //1. read 
    if(_modbus){
        unsigned short val_di = 0x0000;
        if(modbus_read_input_registers(_modbus, 48, 1, &val_di)!=-1){
            console::info("DI Value : {}", val_di);
            for(auto& di:_di_container){
                _di_values[di.first] = static_cast<bool>(val_di&(0x0001<<di.first));
                console::info("DI({}) : {}", di.first, _di_values[di.first]);
            }
        }
        else{
            console::info("Modbus Error : {}", modbus_strerror(errno));
        }
    }

}


void moxaIoServiceTask::cleanup(){

    if(_modbus){
        modbus_close(_modbus);
        modbus_free(_modbus);
    }

    //UDP connection close
    shutdown(_sockfd, SHUT_RDWR);
	close(_sockfd);
}

void moxaIoServiceTask::pause(){

}

void moxaIoServiceTask::resume(){

}

