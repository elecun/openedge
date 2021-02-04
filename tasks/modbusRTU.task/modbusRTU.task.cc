
#include "modbusRTU.task.hpp"

//static component instance that has only single instance
static modbusRTUTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new modbusRTUTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}



bool modbusRTUTask::configure(){
    spdlog::info("create pull");
    _pull = zmq::zsock_new_pull("inproc://example");
    return true;
}

void modbusRTUTask::execute(){
    char* _string = zmq::zstr_recv(_pull);
    spdlog::info("ZMQ pull data : {}", _string);
    //puts(_string);
    zmq::zstr_free(&_string);
}

void modbusRTUTask::cleanup(){
    zmq::zsock_destroy(&_pull);
}

void modbusRTUTask::pause(){

}

void modbusRTUTask::resume(){

}