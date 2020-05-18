

#include "mongodb.connector.service.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>
#include <string>
#include <3rdparty/json.hpp>
#include <3rdparty/mongoc/mongoc.h>

using namespace std::chrono;
using json = nlohmann::json;
using namespace std;

//static service
static mongodbConnectorService* _instance = nullptr;
oe::core::iService* create(){ if(!_instance) _instance = new mongodbConnectorService(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

mongodbConnectorService::mongodbConnectorService()
{
}

mongodbConnectorService::~mongodbConnectorService(){
    closeService();
}

bool mongodbConnectorService::closeService(){
    spdlog::info("close mongoConnectorService");

    mongoc_cleanup();

    return true;
}

bool mongodbConnectorService::initService(const char* config){
    assert(config!=nullptr);

    spdlog::info("service config : {}", config);

    mongoc_init();

    //read configuration for fenet connection
    try {
        json conf;
        conf = json::parse(config);
        if(conf.find("connection")==conf.end())
            return false;

        if(conf["connection"].find("address")!=conf["connection"].end())
            _mongodb_address = conf["connection"]["address"].get<std::string>();  //extract ip4v address
        if(conf["connection"].find("port")!=conf["connection"].end())
            _mongodb_port = conf["connection"]["port"].get<int>();                //extract port
        spdlog::info("MongoDB Connection : {}:{}", _mongodb_address, _mongodb_port);
    }
    catch(const json::exception& e){
        spdlog::error("service profile : {}", e.what());
    }


    //add service
    service->Add("test", jsonrpccxx::GetHandle(&mongodbConnectorService::test, *this), {"value"});

    return true;
}

bool mongodbConnectorService::test(const int& value){
    spdlog::info("call test : {}", value);
    return true;
}