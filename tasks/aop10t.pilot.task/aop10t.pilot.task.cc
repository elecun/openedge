

#include "aop10t.pilot.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <string>

using namespace std;

//task create & release
static aop10tPilotTask* _instance = nullptr;
core::task::runnable* create(){
    if(!_instance)
        _instance = new aop10tPilotTask();
    return _instance;
}

void release(){
    if(_instance){
        delete _instance;
        _instance = nullptr;
    }
}

bool aop10tPilotTask::configure(){

    vector<string> services = getProfile()->data["services"];

    for(string srv: services)
        getProfile()->data["services"][srv.c_str()]

    return true;
}

void aop10tPilotTask::execute(){
    
}

void aop10tPilotTask::cleanup(){
    
}
