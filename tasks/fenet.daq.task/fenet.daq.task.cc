

#include "fenet.daq.task.hpp"
#include <3rdparty/spdlog/spdlog.h>


using namespace std;

//task create & release
static fenetDaqTask* _instance = nullptr;
core::task::runnable* create(){
    if(!_instance)
        _instance = new fenetDaqTask();
    return _instance;
}

void release(){
    if(_instance){
        delete _instance;
        _instance = nullptr;
    }
}

bool fenetDaqTask::configure(){

    return true;
}

void fenetDaqTask::execute(){
    
}

void fenetDaqTask::cleanup(){
    
}
