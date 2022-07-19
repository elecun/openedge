
#include "device.dx3000.motor.hpp"
#include <openedge/log.hpp>
#include <openedge/device/bus.hpp>
#include <openedge/device/controller.hpp>
#include <support/device/dkm/dx3000.hpp>

//static component instance that has only single instance
static device_dx3000_motor* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new device_dx3000_motor(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

using namespace oe::support;

device_dx3000_motor::device_dx3000_motor(){


}

device_dx3000_motor::~device_dx3000_motor(){

}

void device_dx3000_motor::execute(){
 
}

void device_dx3000_motor::stop(){

}

bool device_dx3000_motor::configure(){

    try {

    }
    catch(const json::exception& e){
        console::error("Profile read/access error : {}", e.what());
    }

    return true;
}

void device_dx3000_motor::cleanup(){

}

void device_dx3000_motor::pause(){
    
}

void device_dx3000_motor::resume(){
    
}

