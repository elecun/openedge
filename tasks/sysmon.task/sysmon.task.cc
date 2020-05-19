

#include "sysmon.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>


//task create & release
static sysmonTask* _instance = nullptr;
oe::core::rt_task::runnable* create(){
    spdlog::info("sysmon.task create()");
    if(!_instance)
        _instance = new sysmonTask();
    return _instance;
}

void release(){
    spdlog::info("sysmon.task release()");
    if(_instance){
        delete _instance;
        _instance = nullptr;
    }
}


sysmonTask::~sysmonTask(){

}

bool sysmonTask::configure(){
    spdlog::info("sysmon.task configure()");
    return true;
}

void sysmonTask::execute(){
    // clock_gettime(CLOCK_REALTIME,&x); 
    // spdlog::info("{}.{:09d}",x.tv_sec, x.tv_nsec);
}

void sysmonTask::cleanup(){
    spdlog::info("sysmon.task cleanup()");
}