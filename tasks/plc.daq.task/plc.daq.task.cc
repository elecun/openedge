

#include "plc.daq.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <openedge/device/plc.hpp>

//task create & release
static plcDaqTask* _instance = nullptr;
oe::core::rt_task::runnable* create(){
    if(!_instance)
        _instance = new plcDaqTask();
    return _instance;
}

void release(){
    if(_instance){
        delete _instance;
        _instance = nullptr;
    }
}


plcDaqTask::~plcDaqTask(){
    
}

bool plcDaqTask::configure(){
    
    //load service file with default configuration
    _serviceContainer[0] = loadService("plc.general.service");  //load general plc service
    _serviceContainer[1] = loadService("bus.ethernet.tcp.service"); //load ethernet TCP service
    _serviceContainer[2] = loadService("protocol.xgt.dedicated.service"); //load ethernet TCP service

    return true;
}

void plcDaqTask::execute(){

    
}

void plcDaqTask::cleanup(){

}