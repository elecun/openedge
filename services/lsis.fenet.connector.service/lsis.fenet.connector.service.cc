

#include "lsis.fenet.connector.service.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>
#include <string>
#include <3rdparty/json.hpp>

using namespace std::chrono;
using json = nlohmann::json;
using namespace std;

//static service
static fenetConnectorService* _instance = nullptr;
oe::core::iService* create(){ if(!_instance) _instance = new fenetConnectorService(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

fenetConnectorService::fenetConnectorService(){
    spdlog::info("construct the fenet connector service");
}

fenetConnectorService::~fenetConnectorService(){
    closeService();
    spdlog::info("destruct the fenet connector service");
}

bool fenetConnectorService::closeService(){
    spdlog::info("close fenetConnectorService");
    if(_fenetConnector.is_connected()){
        _fenetConnector.close();
        spdlog::info("Closed FENet Connector");
    }
    return true;
}

bool fenetConnectorService::initService(const char* config){
    assert(config!=nullptr);

    sockpp::socket_initializer sockInit; //initialize socket
    _serviceport = make_unique<rpc::server>(50000);

    try {
        json conf;
        conf = json::parse(config);

        if(conf.find("connection")==conf.end())
            return false;

        if(conf["connection"].find("address")!=conf["connection"].end())
            _fenet_address = conf["connection"]["address"].get<std::string>();  //extract ip4v address
        if(conf["connection"].find("port")!=conf["connection"].end())
            _fenet_port = conf["connection"]["port"].get<int>();                //extract port
        if(conf["connection"].find("timeout")!=conf["connection"].end())
            _fenet_timeout = conf["connection"]["timeout"].get<unsigned long long>();
        spdlog::info("FEnet Connection : {}:{}", _fenet_address, _fenet_port);
    }
    catch(const json::exception& e){
        spdlog::error("service profile : {}", e.what());
    }
    //connect
    _fenetConnector.connect({_fenet_address, static_cast<in_port_t>(_fenet_port)});
    if(!_fenetConnector.is_connected()){
        spdlog::warn("FENet Connection Error : {}", _fenetConnector.last_error_str());
    }
    if(!_fenetConnector.read_timeout(std::chrono::nanoseconds(_fenet_timeout)))
        spdlog::warn("Setting FENet read timeout failed");

    spdlog::info("Opened FENet Connection : {}", _fenetConnector.address().to_string());

    return true;
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