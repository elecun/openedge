
#include "agw.manage.task.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static agwManageTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new agwManageTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool agwManageTask::configure(){
    console::info("call agwManageTask configure()");
    return true;
}

void agwManageTask::execute(){
    console::info("call agwManageTask execute()");
}

void agwManageTask::cleanup(){
    console::info("call agwManageTask cleanup()");
}

void agwManageTask::pause(){
    console::info("call agwManageTask pause()");
}

void agwManageTask::resume(){
    console::info("call agwManageTask resume()");
}
