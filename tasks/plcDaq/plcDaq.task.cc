

#include "plcDaq.task.hpp"
#include <3rdparty/spdlog/spdlog.h>


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
    return true;
}

void plcDaqTask::execute(){
    // clock_gettime(CLOCK_REALTIME,&x); 
    // spdlog::info("{}.{:09d}",x.tv_sec, x.tv_nsec);
    
}

void plcDaqTask::cleanup(){

}