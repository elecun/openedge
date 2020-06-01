

#include "mqtt.publisher.service.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>
#include <string>
#include <3rdparty/json.hpp>
#include <string>
#include "mqtt.hpp"

using namespace std::chrono;
using json = nlohmann::json;
using namespace std;

//static service
static mqttPublisherService* _instance = nullptr;
oe::core::iService* create(){ if(!_instance) _instance = new mqttPublisherService(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

mqttPublisherService::mqttPublisherService()
{

}

mqttPublisherService::~mqttPublisherService(){
    closeService();
}

bool mqttPublisherService::closeService(){

    spdlog::info("close mqttPublisherService");

    return true;
}

bool mqttPublisherService::initService(const char* config){
    assert(config!=nullptr);

    spdlog::info("service config : {}", config);

    //read configuration for fenet connection
    try {
        json conf;
        conf = json::parse(config);
        if(conf.find("connection")==conf.end())
            return false;

        if(conf["connection"].find("address")!=conf["connection"].end())
            _mqtt_address = conf["connection"]["address"].get<std::string>();  //extract ip4v address
        spdlog::info("MQTT Connection : {}", _mqtt_address);

        if(conf["info"].find("topic")!=conf["info"].end())
            _topic = conf["info"]["topic"].get<string>();

    }
    catch(const json::exception& e){
        spdlog::error("service profile : {}", e.what());
    }

    //add service
    service->Add("publish", jsonrpccxx::GetHandle(&mqttPublisherService::publish, *this), {"data"});

    if(!_mqtt)
        _mqtt = make_unique<mqttClient>("data", _mqtt_address.c_str(), 1883, _topic.c_str());

	if(_mqtt->connected())
        spdlog::info("MQTT Connected");
    else
        return false;

    return true;
}

bool mqttPublisherService::publish(const string& topic, const string& data){

    try {
        
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