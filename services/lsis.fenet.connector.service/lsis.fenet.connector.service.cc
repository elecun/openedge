

#include "lsis.fenet.connector.service.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>
#include <string>
#include "xgt_protocol.hpp"

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

    //_protocol = make_unique<oe::bus::protocol::XGTDedicated>();

    //read configuration for fenet connection
    string _fenet_address {""};
    int _fenet_port {0};
    unsigned long long _fenet_timeout {0}; //read timeout
    try {
        serviceConfig = json::parse(config);
        if(serviceConfig.find("connection")==serviceConfig.end() || serviceConfig.find("info")==serviceConfig.end())
            return false;

        if(serviceConfig["connection"].find("address")!=serviceConfig["connection"].end())
            _fenet_address = serviceConfig["connection"]["address"].get<std::string>();  //extract ip4v address
        if(serviceConfig["connection"].find("port")!=serviceConfig["connection"].end())
            _fenet_port = serviceConfig["connection"]["port"].get<int>();                //extract port
        if(serviceConfig["connection"].find("timeout")!=serviceConfig["connection"].end())
            _fenet_timeout = serviceConfig["connection"]["timeout"].get<unsigned long long>();

        _protocol->setParameters(serviceConfig["info"].dump());

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
    service->Add("read", jsonrpccxx::GetHandle(&fenetConnectorService::read, *this), {"address"});
    service->Add("read_n", jsonrpccxx::GetHandle(&fenetConnectorService::read_n, *this), {"address", "count"});

    spdlog::info("Sucessfully created FENet connection : {}", _fenetConnector.address().to_string());

    return true;
}

bool fenetConnectorService::test(const int& value){
    spdlog::info("call test : {}", value);
    return true;
}

string fenetConnectorService::read(const std::string& address){
    spdlog::info("call request address : {}", address);


    return string("");
}

string fenetConnectorService::read_n(const std::string& address, int count){
    spdlog::info("call request address: {} with {}", address, count);

    //request to read block
    // using oe::bus::protocol;
    // uint16_t varlen = static_cast<uint16_t>(address.size());
    // uint16_t blocks = 0x0001;   //single block
    // string raddr = address.substr(3); //sub string of address
    // _protocol->request(XGTDedicated::command_code_t::READ_REQUEST,
    //                     XGTDedicated::datatype_t::BLOCK,
    //                     )
    // vector<uint8_t> frame = _protocol->getFrame();

    return string("");
}