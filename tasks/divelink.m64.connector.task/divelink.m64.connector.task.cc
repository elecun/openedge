
#include "divelink.m64.connector.task.hpp"
#include <openedge/log.hpp>
#include <openedge/common/uart.hpp>

//static component instance that has only single instance
static divelink_m64_connector_task* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new divelink_m64_connector_task(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

divelink_m64_connector_task::divelink_m64_connector_task(){


}

divelink_m64_connector_task::~divelink_m64_connector_task(){

}

void divelink_m64_connector_task::execute(){

}

void divelink_m64_connector_task::stop(){

}

bool divelink_m64_connector_task::configure(){

    if(_com){
        _com->close();
    }

    const char* device = "/dev/ttyUSB0";
    _com  = new bus::sync_uart(device);
    if(!_com->open()){
        console::error("{} bus cannot be opened", device);
        return false;
    }
    
}

void divelink_m64_connector_task::cleanup(){
    if(_com){
        _com->close();
    }
}

void divelink_m64_connector_task::pause(){
    
}

void divelink_m64_connector_task::resume(){
    
}

void divelink_m64_connector_task::on_request(std::any data){

}
