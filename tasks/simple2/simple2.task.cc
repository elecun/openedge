

#include "simple2.task.hpp"
#include <external/spdlog/spdlog.h>


//task create & release
static simpletask2* _instance = nullptr;
oe::core::rt_task::runnable* create(){
    spdlog::info("simple2.task create()");
    if(!_instance)
        _instance = new simpletask2();
    return _instance;
}

void release(){
    spdlog::info("simple2.task release()");
    if(_instance){
        delete _instance;
        _instance = nullptr;
    }
}


simpletask2::~simpletask2(){

}

bool simpletask2::configure(){
    spdlog::info("simple2.task configure()");
    return true;
}

void simpletask2::execute(){
    // clock_gettime(CLOCK_REALTIME,&x); 
    // spdlog::info("{}.{:09d}",x.tv_sec, x.tv_nsec);
}

void simpletask2::cleanup(){
    spdlog::info("simple2.task cleanup()");
}