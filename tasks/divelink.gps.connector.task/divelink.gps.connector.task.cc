
#include "divelink.gps.connector.task.hpp"
#include <openedge/log.hpp>
#include <openedge/common/uart.hpp>

//static component instance that has only single instance
static divelink_gps_connector_task* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new divelink_gps_connector_task(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

divelink_gps_connector_task::divelink_gps_connector_task(){


}

divelink_gps_connector_task::~divelink_gps_connector_task(){

}

void divelink_gps_connector_task::execute(){

}

void divelink_gps_connector_task::stop(){

}

bool divelink_gps_connector_task::configure(){
    
}

void divelink_gps_connector_task::cleanup(){
    
}

void divelink_gps_connector_task::pause(){
    
}

void divelink_gps_connector_task::resume(){
    
}

void divelink_gps_connector_task::on_request(std::any data){

}
