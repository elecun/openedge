

#include "simple.task.hpp"
#include <external/spdlog/spdlog.h>


//task create & release
static simpletask* _instance = nullptr;
oe::core::rt_task::runnable* create(){
    spdlog::info("simple.task create()");
    if(!_instance)
        _instance = new simpletask();
    return _instance;
}

void release(){
    spdlog::info("simple.task release()");
    if(_instance){
        delete _instance;
        _instance = nullptr;
    }
}


simpletask::~simpletask(){

}

bool simpletask::configure(){
    spdlog::info("simple.task configure()");
    return true;
}

void simpletask::execute(){
    clock_gettime(CLOCK_REALTIME,&x); 
    printf("%ld.%09ld \n",x.tv_sec, x.tv_nsec);
}

void simpletask::cleanup(){
    spdlog::info("simple.task cleanup()");
}