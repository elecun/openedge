

#include "simple.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>


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