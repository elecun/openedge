
#include "dx3000.control.task.hpp"
#include <openedge/log.hpp>

using namespace oe::support;

//static component instance that has only single instance
static dx3000ControlTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new dx3000ControlTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool dx3000ControlTask::configure(){

    return true;
}

void dx3000ControlTask::execute(){
    

}

void dx3000ControlTask::cleanup(){
    
}

void dx3000ControlTask::pause(){

}

void dx3000ControlTask::resume(){

}

