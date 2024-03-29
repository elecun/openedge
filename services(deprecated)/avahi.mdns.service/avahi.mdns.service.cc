

#include "avahi.mdns.service.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>
#include <string>
#include <3rdparty/json.hpp>
#include <string>


using namespace std::chrono;
using json = nlohmann::json;
using namespace std;

//static service
static avahiMdnsService* _instance = nullptr;
oe::core::iService* create(){ if(!_instance) _instance = new avahiMdnsService(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

avahiMdnsService::avahiMdnsService()
{

}

avahiMdnsService::~avahiMdnsService(){
    closeService();
}

bool avahiMdnsService::closeService(){

    return true;
}

bool avahiMdnsService::initService(const char* config){
    assert(config!=nullptr);

    spdlog::info("service config : {}", config);

    //read configuration for fenet connection
    try {
        json conf;
        conf = json::parse(config);
        if(conf.find("discovery")==conf.end())
            return false;

        #define DISCOVERY   conf["discovery"]



        if(DISCOVERY.find("address")!=conf["connection"].end())
            _mqtt_address = conf["connection"]["address"].get<std::string>();  //extract ip4v address
        spdlog::info("MQTT Connection : {}", _mqtt_address);

        if(conf["info"].find("topic")!=conf["info"].end())
            _topic = conf["info"]["topic"].get<string>();

    }
    catch(const json::exception& e){
        spdlog::error("service profile : {}", e.what());
    }

    //add service
    service->Add("publish", jsonrpccxx::GetHandle(&avahiMdnsService::publish, *this), {"data"});
    service->Add("drop", jsonrpccxx::GetHandle(&avahiMdnsService::drop, *this), {"data"});

    return true;
}

bool avahiMdnsService::publish(const string& data){

    try {
        //find service


        int rc = _mqtt->publish(0, _topic.c_str(), strlen(data.c_str()), data.c_str(), 2, false);
		rc = _mqtt->loop();
        if(rc)
			_mqtt->reconnect();
		usleep(1e5);
    }
    catch(json::exception& e){
        spdlog::error("Data Publish error : {}", e.what());
    }
    return true;
}

bool avahiMdnsService::drop(){
    return true;
}