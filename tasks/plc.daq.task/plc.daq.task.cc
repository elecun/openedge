

#include "plc.daq.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <openedge/core/service.hpp>



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
    delete _bus;
    delete _plc;
}

bool plcDaqTask::configure(){
    if(!_plc) _plc = new lsisPlc();
    if(!_bus) _bus = new fenet("192.168.11.100", 2004);

    if(!_plc->connect_with(_bus))
        return false;

    _plc->read()

    return true;
}

void plcDaqTask::execute(){


    
}

void plcDaqTask::cleanup(){

}