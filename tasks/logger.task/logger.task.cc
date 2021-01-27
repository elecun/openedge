

#include "logger.task.hpp"
#include <support/device/general.hpp>

using namespace std;

//static component instance that has only single instance
static loggerTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new loggerTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}



bool loggerTask::configure(){
    //device instance
    json _system = json::parse(getProfile()->get("system"));
    string _bus = _system["bus"].get<string>();
    spdlog::info("use system bus : {}", _bus);

    json _device = json::parse(getProfile()->get("device"));
    vector<string> _required = _device["required"].get<vector<string>>();

    for(string& dev: _required){
        unsigned char address = static_cast<unsigned char>(_device[dev]["address"].get<int>());
        vector<int> _register = _device[dev]["register"].get<vector<int>>();

        //_device.emplace_back(new oe::generalDevice(new oe::))
    }
    
    return true;
}

void loggerTask::execute(){
    spdlog::info("execute");
}

void loggerTask::cleanup(){
    spdlog::info("cleanup");
}
