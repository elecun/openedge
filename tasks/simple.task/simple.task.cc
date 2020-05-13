

#include "simple.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>


//task create & release
static simpletask* _instance = nullptr;
oe::core::task::runnable* create(){
    if(!_instance)
        _instance = new simpletask();
    return _instance;
}

void release(){
    if(_instance){
        delete _instance;
        _instance = nullptr;
    }
}


bool simpletask::configure(){
    return true;
}

void simpletask::execute(){
    spdlog::info("do simple task execute");
    // clock_gettime(CLOCK_REALTIME,&x); 
    // spdlog::info("{}.{:09d}",x.tv_sec, x.tv_nsec);
    
    // auto t_start = std::chrono::high_resolution_clock::now();
    // spdlog::info("execute");
    // auto t_end = std::chrono::high_resolution_clock::now();
    // using fpsec = std::chrono::duration<double, std::chrono::seconds::period>;
    // spdlog::info("Elpased : {}", fpsec(t_end - t_start).count());
    
}

void simpletask::cleanup(){
    spdlog::info("simple.task cleanup()");
}