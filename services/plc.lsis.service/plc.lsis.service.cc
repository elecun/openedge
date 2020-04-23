

#include "plc.lsis.service.hpp"

plcLsisService::plcLsisService() {

}

plcLsisService::~plcLsisService(){

}

template<typename _returnType, UnsignedOnly<_returnType>>
_returnType plcLsisService::read(const char* address){
    uint8_t test = 0x01;
    return static_cast<_returnType>(test);
}

uint8_t plcLsisService::readByte(const char* address){
    return 0x01;
}