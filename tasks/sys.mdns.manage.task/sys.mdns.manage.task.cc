

#include "sys.mdns.manage.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <3rdparty/tinyxml2/tinyxml2.h>


//implemented create/release singleton instance
static sysMdnsManageTask* _instance = nullptr;
oe::core::task::runnable* create(){
    if(!_instance)
        _instance = new sysMdnsManageTask();
    return _instance;
}

void release(){
    if(_instance){
        delete _instance;
        _instance = nullptr;
    }
}


bool sysMdnsManageTask::configure(){
    spdlog::info("call configure");
    return true;
}

void sysMdnsManageTask::execute(){
    spdlog::info("call execute");
}

void sysMdnsManageTask::cleanup(){
    spdlog::info("call cleanup");
}