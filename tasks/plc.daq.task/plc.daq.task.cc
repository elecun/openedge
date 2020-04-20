

#include "plc.daq.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <openedge/core/service.hpp>
#include <openedge/services/plc.general.service/plc.general.service.hpp>


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

    _plc->close();

    if(_bus)
        delete _bus;
    if(_plc)
        delete _plc;

}

bool plcDaqTask::configure(){

    if(!_plc->open())
        return false;

    _plc->addModule(0, new )

    //_bus = new bus;
    //_plc = new plc(with bus interface)
    //_plc->connect();

    return true;
}

void plcDaqTask::execute(){

    //request
    //_plc->readBytes()
    //_plc->readByte()
    
}

void plcDaqTask::cleanup(){

}