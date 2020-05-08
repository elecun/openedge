

#include "fenet.connector.service.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>

using namespace std::chrono;

fenetConnectorService::fenetConnectorService(){

}

fenetConnectorService::~fenetConnectorService(){
    if(_sockInit)
        delete _sockInit;
}

bool fenetConnectorService::initService(){
    if(_bus.get()!=nullptr){
        spdlog::warn("Interface Bus has already assigned");
        return false;
    }

    if(!_sockInit){
        _sockInit = new sockpp::socket_initializer();
    }

    _bus = make_unique<core::bus::iDeviceBusTCP>();
    if(_bus.get()){

    }

    return true;
}


void fenetConnectorService::request(const char* addr_start, uint16_t count){
    if(_bus.get()){

    }
}

void fenetConnectorService::parse(const char* address){

}

bool fenetConnectorService::connect(const char* ipv4_addr, int port){
    in_port_t p = static_cast<in_port_t>(port);
    _tcp.connect({ipv4_addr, p});

    if(!_tcp){
        spdlog::error("Socket Connection Error : {}", _tcp.last_error_str());
    }
    return true;
}

void fenetConnectorService::setRcvTimeout(unsigned int sec){
    if(!_tcp.read_timeout(seconds(sec))){
        spdlog::error("Cannot set timeout on TCP Stream : {}", _tcp.last_error_str());
    }
}

void fenetConnectorService::setRcvCallback(std::function<int(const vector<byte>&)> packet){

}