
#include "divelink.m64.control.task.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static divelink_m64_control_task* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new divelink_m64_control_task(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

divelink_m64_control_task::divelink_m64_control_task(){


}

divelink_m64_control_task::~divelink_m64_control_task(){

}

void divelink_m64_control_task::execute(){

}

void divelink_m64_control_task::stop(){

}

bool divelink_m64_control_task::configure(){
    
}

void divelink_m64_control_task::cleanup(){
    
}

void divelink_m64_control_task::pause(){
    
}

void divelink_m64_control_task::resume(){
    
}

void divelink_m64_control_task::on_request(std::any data){

}
