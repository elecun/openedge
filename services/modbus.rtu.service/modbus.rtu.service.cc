

#include "modbus.rtu.service.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>
#include <string>
#include <3rdparty/json.hpp>
#include <3rdparty/libmodbus/modbus.h>
#include <3rdparty/libmodbus/modbus-rtu.h>

using namespace std::chrono;
using json = nlohmann::json;
using namespace std;

//static service
static modbusRtuService* _instance = nullptr;
oe::core::iService* create(){ if(!_instance) _instance = new modbusRtuService(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

modbusRtuService::modbusRtuService()
{
}

modbusRtuService::~modbusRtuService(){
    closeService();
}

bool modbusRtuService::closeService(){

    spdlog::info("close modbus RTU Service");

    return true;
}

bool modbusRtuService::initService(const char* config){
    assert(config!=nullptr);

    spdlog::info("service config : {}", config);

    //read configuration for fenet connection
    try {
        json conf;
        conf = json::parse(config);
        if(conf.find("connection")==conf.end())
            return false;

            #define CONNECTION  conf["connection"]
            #define INFO  conf["info"]

        if(CONNECTION.find("port")!=CONNECTION.end())
            _port = CONNECTION["port"].get<std::string>(); //device port
        if(CONNECTION.find("baudrate")!=CONNECTION.end())
            _baudrate = CONNECTION["baudrate"].get<unsigned int>(); //baudrate
        spdlog::info("Connection : {}:{}", _port, _baudrate);

    }
    catch(const json::exception& e){
        spdlog::error("service profile : {}", e.what());
    }

    //add service
    service->Add("read", jsonrpccxx::GetHandle(&modbusRtuService::read, *this), {"address"});

    return true;
}

unsigned char modbusRtuService::read(const unsigned char address /*json*/){

    try {
        
    }
    catch(json::exception& e){
        spdlog::error("Document insertion error : {}", e.what());
    }
    return true;
}