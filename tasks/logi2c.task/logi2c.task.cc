

#include "logi2c.task.hpp"

using namespace std;

//static component instance that has only single instance
static logi2cTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new logi2cTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}



bool logi2cTask::configure(){

    return true;
}

void logi2cTask::execute(){
    
}

void logi2cTask::cleanup(){

    
}
