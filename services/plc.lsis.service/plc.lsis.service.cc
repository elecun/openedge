

#include "plc.lsis.service.hpp"
#include <3rdparty/spdlog/spdlog.h>

plcLsisService::plcLsisService() {
    
}

plcLsisService::~plcLsisService(){
    
}

bool plcLsisService::initService(){
    return true;
}


bool plcLsisService::readBit(bus::iDeviceBus* bus, iProtocolRaw* protocol, const char* address){

    return false;
}

uint8_t plcLsisService::readByte(bus::iDeviceBus* bus, iProtocolRaw* protocol, const char* address){
    return 0x01;
}

uint16_t plcLsisService::readWord(bus::iDeviceBus* bus, iProtocolRaw* protocol, const char* address){
    return 0x0000;
}

uint32_t plcLsisService::readDword(bus::iDeviceBus* bus, iProtocolRaw* protocol, const char* address){
    return 0x000000;
}

uint64_t plcLsisService::readLword(bus::iDeviceBus* bus, iProtocolRaw* protocol, const char* address){
    return 0x00000000;
}




// uint8_t plcLsisService::readByte(const char* address){
//     if(_bus){
//         uint8_t data[] = { 0x4C, 0x53, 0x49, 0x53, 0x2D, 0x58, 0x47, 0x54 };
//         _bus->read(data, sizeof(data)); //for test
//     }
//     return 0x05;
// }


bool plcLsisService::open(){
    return true;
}

void plcLsisService::close(){

}