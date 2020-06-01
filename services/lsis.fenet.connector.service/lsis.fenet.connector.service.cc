

#include "lsis.fenet.connector.service.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>
#include <vector>
#include <string>
#include "xgt.protocol.hpp"
#include <stdexcept>

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

        //connection profile
        json connection = serviceConfig["connection"];
        if(connection.find("address")!=connection.end())
            _fenet_address = connection["address"].get<std::string>();  //extract ip4v address
        if(connection.find("port")!=connection.end())
            _fenet_port = connection["port"].get<int>();                //extract port
        if(connection.find("timeout")!=connection.end())
            _fenet_timeout = connection["timeout"].get<unsigned long long>();

        //info profile

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
    service->Add("read_block", jsonrpccxx::GetHandle(&fenetConnectorService::read_block, *this), {"address", "count"});

    spdlog::info("Sucessfully created FENet connection : {}", _fenetConnector.address().to_string());

    return true;
}

string fenetConnectorService::read(const std::string& address){
    spdlog::info("call request address : {}", address);

    return string("");
}

vector<uint8_t> fenetConnectorService::read_block(const std::string& address, int size){

    vector<uint8_t> rawdata;

    try {
        assert(address.at(0)=='%');

        const char memprefix[] = { 'I', 'Q', 'M', 'R', 'W'};
        char mem {0};
        for(auto m:memprefix){
            if(address.at(1)==m){
                mem = m; break;
            }
        }
        assert(mem!=0);

        const char typeprefix[] = {'X', 'B', 'W', 'D', 'L'};
        char type{0};
        for(auto t:typeprefix){
            if(address.at(2)==t){
                type = t; break;
            }
        }
        assert(type!=0);

        vector<uint8_t> packet = _protocol->gen_read_block(address, size);
        string strpack = _vec2str(packet);
        spdlog::info("Generated({}) : {}", packet.size(), strpack);

        if(_fenetConnector.is_connected()){
            int sent = _fenetConnector.write(&packet[0], packet.size());
            
            unsigned char data[1024] = {0,};
            int received = _fenetConnector.read_n(data, sizeof(data));
            if(received>0){
                spdlog::info("Received({}) : {}", received, _c2str(data, received));
                using oe::bus::protocol::xgt_errorcode_t;
                xgt_errorcode_t errorcode = _protocol->checkError(data, received);
                if(errorcode!=xgt_errorcode_t::NORMAL_OPERATION){
                    spdlog::error("Error Code : {:x}", static_cast<uint16_t>(errorcode));
                }
                else{
                    rawdata.assign(data+(received-size), data+received);
                }
            }
        }
    }
    catch(std::out_of_range& e){
        spdlog::error("{}",e.what());
    }

    return rawdata;
}

string fenetConnectorService::_vec2str(vector<uint8_t>& data){
    string str;
    for(uint8_t d:data)
        str.append(fmt::format("{:x} ", d));
    return str;
}

string fenetConnectorService::_c2str(const unsigned char* data, int size){
    string str;
    for(int i=0;i<size;i++)
        str.append(fmt::format("{:x} ", data[i]));
    return str;
}
