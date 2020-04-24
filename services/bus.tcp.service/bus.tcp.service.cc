

#include "bus.tcp.service.hpp"
#include <3rdparty/spdlog/spdlog.h>

busTcpService::busTcpService() {
    
}

busTcpService::~busTcpService(){
    if(_asyncSocket)
        delete _asyncSocket;
}

bool busTcpService::initService(){
    _asyncSocket = new net::socket();
    return _asyncSocket->connect("192.168.11.9", 2004);
}


bool busTcpService::connect(const char* ipv4_address, const int port){
    if(_asyncSocket)
        return true;

    return initService();
}

void busTcpService::disconnect(){

}

bool busTcpService::open(){
    return true;
}

void busTcpService::close(){

}

int busTcpService::read(uint8_t* data, int len){
    if(_asyncSocket)
        _asyncSocket->send(data, len);
        
    return 0;
}

int busTcpService::write(const uint8_t* data, int len){
    return 0;
}