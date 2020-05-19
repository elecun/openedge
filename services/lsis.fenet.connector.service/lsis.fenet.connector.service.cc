

#include "lsis.fenet.connector.service.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>
#include <string>
#include "xgt.protocol.hpp"

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

    _protocol = make_unique<oe::bus::protocol::XGTDedicated>();

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

        if(_protocol)
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
    service->Add("read", jsonrpccxx::GetHandle(&fenetConnectorService::read, *this), {"address"});
    service->Add("read_n", jsonrpccxx::GetHandle(&fenetConnectorService::read_n, *this), {"address", "count"});

    spdlog::info("Sucessfully created FENet connection : {}", _fenetConnector.address().to_string());

    return true;
}

string fenetConnectorService::read(const std::string& address){
    spdlog::info("call request address : {}", address);

    return string("");
}

string fenetConnectorService::read_n(const std::string& address, int count){
    //spdlog::info("call request to read block: {} with {}", address, count);

    char address_array[address.size()];
    std::copy(address.begin(), address.end(), address_array);

    if(address_array[0]!='%'){
        spdlog::error("Invalid FEnet address format : {}", address);
        return string("{}");
    }

    if(address_array[2]=='B'){ //Word
        vector<uint8_t> packet = _protocol->gen_read_block(address, count);
        string packet_str;
        for(uint8_t d:packet)
            packet_str.append(fmt::format("{:x} ", d));
        spdlog::info("Generated Packet({}) : {}", packet.size(), packet_str);

        int size = _fenetConnector.write(&packet[0], packet.size());

        //std::this_thread::sleep_for(std::chrono::nanoseconds(20000000));

        unsigned char data[100] = {0,};
        int rsize = _fenetConnector.read_n(data, sizeof(data));

        if(rsize>0){
            string read;
            for(int i=0;i<rsize;i++)
                read.append(fmt::format("{:x} ", data[i]));
            spdlog::info("Received Packet({}) : {}", rsize, read);

            uint16_t error = _protocol->check_response_error(data, rsize);
            if(error)
                spdlog::error("Protocol Error : {:x}", error);
        }
    }

    //_fenetConnector.write();

    // switch(address.at(1)){
    //     case 'W':   // Word (16bit)
    //         {
    //             string start_address = address.substr(3);
    //             vector<uint16_t> data = _protocol->read_block<uint16_t>(start_address, count);
    //         }
    //     break;
    // }

    return string("{}");
}