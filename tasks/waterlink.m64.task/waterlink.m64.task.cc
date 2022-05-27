
#include "waterlink.m64.task.hpp"
#include <support/device/waterlinked/m64.hpp>
#include <openedge/common/uart.hpp>
#include <openedge/log.hpp>

//static component instance that has only single instance
static waterlinkM64Task* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new waterlinkM64Task(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


/* internal static instance */
static oe::interface::bus* _bus = new oe::busUART(iDeviceBusUART::);
static oe::device::waterlink::m64* _m64 = new oe::device::waterlink::m64(_bus);

waterlinkM64Task::waterlinkM64Task(){
    
}

waterlinkM64Task::~waterlinkM64Task(){
    delete _bus;
    delete _m64;
}

bool waterlinkM64Task::configure(){
    return true;
}

void waterlinkM64Task::execute(){
    
}

void waterlinkM64Task::cleanup(){
    
}

void waterlinkM64Task::pause(){

}

void waterlinkM64Task::resume(){

}

void waterlinkM64Task::on_request(std::any data){

}
