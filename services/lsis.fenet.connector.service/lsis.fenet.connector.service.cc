

#include "lsis.fenet.connector.service.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>
#include <string>

using namespace std::chrono;
using namespace std;

//static service
static fenetConnectorService* _instance = nullptr;
oe::core::iService* create(){ if(!_instance) _instance = new fenetConnectorService(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

fenetConnectorService::fenetConnectorService()
{
}

fenetConnectorService::~fenetConnectorService(){
    closeService();
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

    sockpp::socket_initializer sockInit; //initialize socket for fenet connection

    //read configuration for fenet connection
    string _fenet_address {""};
    int _fenet_port {0};
    unsigned long long _fenet_timeout {0}; //read timeout
    try {
        serivceConfig = json::parse(config);
        if(serivceConfig.find("connection")==serivceConfig.end())
            return false;

        if(serivceConfig["connection"].find("address")!=serivceConfig["connection"].end())
            _fenet_address = serivceConfig["connection"]["address"].get<std::string>();  //extract ip4v address
        if(serivceConfig["connection"].find("port")!=serivceConfig["connection"].end())
            _fenet_port = serivceConfig["connection"]["port"].get<int>();                //extract port
        if(serivceConfig["connection"].find("timeout")!=serivceConfig["connection"].end())
            _fenet_timeout = serivceConfig["connection"]["timeout"].get<unsigned long long>();
        spdlog::info("FEnet Connection : {}:{}", _fenet_address, _fenet_port);
    }
    catch(const json::exception& e){
        spdlog::error("service profile : {}", e.what());
    }

    //fenet connect
    _fenetConnector.connect({_fenet_address, static_cast<in_port_t>(_fenet_port)});
    if(!_fenetConnector.is_connected()){
        spdlog::error("FENet Connection Error : {}", _fenetConnector.last_error_str());
    }
    //set read timeout
    if(!_fenetConnector.read_timeout(std::chrono::nanoseconds(_fenet_timeout)))
        spdlog::warn("Setting FENet read timeout failed");

    //register function to provide services
    service->Add("test", jsonrpccxx::GetHandle(&fenetConnectorService::test, *this), {"value"});
    service->Add("write", jsonrpccxx::GetHandle(&fenetConnectorService::write, *this), {"address"});
    service->Add("write_n", jsonrpccxx::GetHandle(&fenetConnectorService::write_n, *this), {"address"});

    spdlog::info("Sucessfully created FENet connection : {}", _fenetConnector.address().to_string());

    return true;
}

bool fenetConnectorService::test(const int& value){
    spdlog::info("call test : {}", value);
    return true;
}

string fenetConnectorService::write(const std::string& address){
    spdlog::info("call request address : {}", address);


    return string("");
}

string fenetConnectorService::write_n(const std::string& address){
    spdlog::info("call request address: {}", address);

    return string("");
}