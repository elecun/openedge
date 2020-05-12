

#include "lsis.fenet.connector.service.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>

using namespace std::chrono;

sockpp::socket_initializer* _sockInit { nullptr };
sockpp::tcp_connector _tcp ;

//static service
static fenetConnectorService* _instance = nullptr;
oe::core::iService* create(){ if(!_instance) _instance = new fenetConnectorService(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

fenetConnectorService::fenetConnectorService(){
    spdlog::info("construct the fenet connector service");
}

fenetConnectorService::~fenetConnectorService(){
    spdlog::info("destruct the fenet connector service");
}

bool fenetConnectorService::closeService(){
    spdlog::info("close fenetConnectorService");
    return true;
}

bool fenetConnectorService::initService(){
    spdlog::info("call init service");
    //spdlog::debug("fenet connector service : init service");
    // if(_bus.get()!=nullptr){
    //     spdlog::warn("Interface Bus has already assigned");
    //     return false;
    // }

    // if(!_sockInit){
    //     _sockInit = new sockpp::socket_initializer();
    // }

    // _bus = make_unique<core::bus::iDeviceBusTCP>();
    // if(_bus.get()){

    // }

    return true;
}


// void fenetConnectorService::request(const char* addr_start, uint16_t count){
//     if(_bus.get()){

//     }
// }

void fenetConnectorService::parse(const char* address){

}

// bool fenetConnectorService::connect(const char* ipv4_address, const int port){
//     in_port_t p = static_cast<in_port_t>(port);
//     return _tcp.connect({ipv4_address, p});
// }

// void fenetConnectorService::disconnect(){
//     if(_tcp.is_connected()){
//         if(_tcp.close())
//             spdlog::info("Successfully closed");
//     }
// }


// void fenetConnectorService::setRcvTimeout(unsigned int sec){
//     if(!_tcp.read_timeout(seconds(sec))){
//         spdlog::error("Cannot set timeout on TCP Stream : {}", _tcp.last_error_str());
//     }
// }

// void fenetConnectorService::setRcvCallback(std::function<int(const vector<byte>&)> packet){

// }