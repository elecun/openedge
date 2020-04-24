

#include "xgt.protocol.service.hpp"
#include <3rdparty/spdlog/spdlog.h>

xgtProtocolService::xgtProtocolService() {
    
}

xgtProtocolService::~xgtProtocolService(){
    
}

bool xgtProtocolService::initService(){
    return true;
}

vector<uint8_t> xgtProtocolService::getHeader(){
    return vector<uint8_t>();
}