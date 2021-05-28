
#include "uvlc.control.task.hpp"


//static component instance that has only single instance
static uvcontrolTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new uvcontrolTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}



bool uvcontrolTask::configure(){
    spdlog::info("create push");
    _push = zmq::zsock_new_push("inproc://example");

    return true;
}

void uvcontrolTask::execute(){
    zstr_send(_push, "Hello, World");
    spdlog::info("push message");
}

void uvcontrolTask::cleanup(){
    zmq::zsock_destroy(&_push);
}

void uvcontrolTask::pause(){

}

void uvcontrolTask::resume(){

}