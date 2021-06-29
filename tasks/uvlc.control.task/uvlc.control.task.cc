
#include "uvlc.control.task.hpp"
#include <openedge/log.hpp>
#include <openedge/device/bus.hpp>
#include <openedge/device/controller.hpp>
#include <support/device/dkm_dx3000.hpp>
#include <openedge/device/gpio.hpp>
#include <support/device/am335x/gpio.hpp>
#include <string>

using namespace oe::support;

//static component instance that has only single instance
static uvlcControlTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new uvlcControlTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool uvlcControlTask::configure(){
    
    return true;
}

void uvlcControlTask::execute(){
    //zstr_send(_push, "Hello, World");
    // spdlog::info("push message");
    
    
}

void uvlcControlTask::cleanup(){
    

    //zmq::zsock_destroy(&_push);
}

void uvlcControlTask::pause(){

}

void uvlcControlTask::resume(){

}