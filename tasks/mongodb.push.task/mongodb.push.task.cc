

#include "mongodb.push.task.hpp"
#include <3rdparty/spdlog/spdlog.h>


using namespace std;

//task create & release
static mongodbPushTask* _instance = nullptr;
core::task::runnable* create(){
    if(!_instance)
        _instance = new mongodbPushTask();
    return _instance;
}

void release(){
    if(_instance){
        delete _instance;
        _instance = nullptr;
    }
}


bool mongodbPushTask::configure(){
    
    return true;
}

void mongodbPushTask::execute(){
    
}

void mongodbPushTask::cleanup(){
    
}
