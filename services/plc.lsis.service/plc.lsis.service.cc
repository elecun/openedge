

#include "plc.lsis.service.hpp"
#include <3rdparty/spdlog/spdlog.h>

plcLsisService::plcLsisService() {
    openService();
}

plcLsisService::~plcLsisService(){
    closeService();
}

bool plcLsisService::openService(){
    spdlog::info("called plcLsisService open");
    return true;
}

void plcLsisService::closeService(){

}

// template<typename _returnType, UnsignedOnly<_returnType>>
// _returnType plcLsisService::read(const char* address){
//     uint8_t test = 0x01;
//     return static_cast<_returnType>(test);
// }


uint8_t plcLsisService::readByte(const char* address){

    return 0x05;
}

bool plcLsisService::configBus(bus::iDeviceBus* bus){
    _bus = bus;
    return true;
}

bool plcLsisService::open(){
    return true;
}

void plcLsisService::close(){

}