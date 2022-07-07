
#include "divelink.m64.connector.task.hpp"
#include <openedge/log.hpp>
#include <openedge/common/uart.hpp>
#include <support/device/waterlinked/m64.hpp>

//static component instance that has only single instance
static divelink_m64_connector_task* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new divelink_m64_connector_task(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

//static instance
static device::waterlink::m64* _m64 = nullptr;

divelink_m64_connector_task::divelink_m64_connector_task(){


}

divelink_m64_connector_task::~divelink_m64_connector_task(){

}

void divelink_m64_connector_task::execute(){
    if(_m64){
        device::waterlink::m64::version v = _m64->get_version();
        console::info("M64 Version : {}", v.str());
    }
    else {
        console::warn("Waterlink M64 device is not working...");
    }
}

void divelink_m64_connector_task::stop(){

}

bool divelink_m64_connector_task::configure(){

    if(_device)
        _device->close();

    try {
        const json& prof = this->get_profile()->get();
        if(prof.contains(PROFILE_CONFIGURATIONS_KEY)){
            json conf = prof[PROFILE_CONFIGURATIONS_KEY];

            if(conf.contains("serial")){
                string port = conf["serial"]["device"].get<string>();
                unsigned int baud = conf["serial"]["baudrate"].get<unsigned int>();

                _device = new bus::sync_uart(port.c_str(), baud);
                if(_device->open()){
                    _m64 = new device::waterlink::m64(_device);
                    console::info("[{}] open device : {}({})", this->get_name(), port, baud);
                }
                else {
                    console::warn("Waterlink M64 bus interface is closed.");
                }
            }
        }

    }
    catch(const json::exception& e){
            console::error("Profile read/access error : {}", e.what());
    }
    return true;
    
}

void divelink_m64_connector_task::cleanup(){
    if(_device){
        _device->close();
        delete _device;
    }

    if(_m64){
        delete _m64;
    }
}

void divelink_m64_connector_task::pause(){
    
}

void divelink_m64_connector_task::resume(){
    
}

void divelink_m64_connector_task::on_request(std::any data){

}
