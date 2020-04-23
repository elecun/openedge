

#include "bus.tcp.service.hpp"
#include <3rdparty/spdlog/spdlog.h>

busTcpService::busTcpService() {
    if(!openService())
        spdlog::error("Cannot connect to the server");
}

busTcpService::~busTcpService(){
    closeService();
}

bool busTcpService::openService(){
    _asyncSocket = new net::socket();

    return _asyncSocket->connect("192.168.11.9", 2004);
}

void busTcpService::closeService(){
    delete _asyncSocket;
}

bool busTcpService::connect(const char* ipv4_address, const int port){
    
    
    return false;
}

void busTcpService::disconnect(){

}

bool busTcpService::open(){
    return true;
}

void busTcpService::close(){

}

int busTcpService::read(uint8_t* data, int len){
    return 0;
}

int busTcpService::write(const uint8_t* data, int len){
    return 0;
}