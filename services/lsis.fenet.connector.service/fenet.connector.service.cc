

#include "fenet.connector.service.hpp"
#include <3rdparty/spdlog/spdlog.h>

fenetConnectorService::fenetConnectorService(){

}

fenetConnectorService::~fenetConnectorService(){
    
}

bool fenetConnectorService::initService(){
    if(_bus.get()!=nullptr){
        spdlog::warn("Interface Bus has already assigned");
        return false;
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

