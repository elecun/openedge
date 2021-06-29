
#include "agsys.manage.task.hpp"
#include <openedge/log.hpp>

using namespace oe::support;

//static component instance that has only single instance
static agsysManageTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new agsysManageTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool agsysManageTask::configure(){

    return true;
}

void agsysManageTask::execute(){
    

}

void agsysManageTask::cleanup(){
    
}

void agsysManageTask::pause(){

}

void agsysManageTask::resume(){

}

