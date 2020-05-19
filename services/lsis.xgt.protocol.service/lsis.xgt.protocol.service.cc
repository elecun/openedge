

#include "xgt.protocol.service.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include "xgt.protocol.hpp"
#include <vector>

using namespace std;

xgtProtocolService::xgtProtocolService() {
    _protocol = new oe::protocol::XGTDedicated();
}

xgtProtocolService::~xgtProtocolService(){
    if(_protocol)
        delete _protocol;
}

bool xgtProtocolService::initService(){
    return true;
}

vector<uint8_t> xgtProtocolService::getHeader(){
    using protocol = oe::protocol::XGTDedicated;
    protocol::xgt_header_t _header;
    _header.setDefault();

    spdlog::info("header size : {}", _header.size());
    

    vector<uint8_t> data(_header.header_frame, _header.header_frame+_header.size());
    return data;
}