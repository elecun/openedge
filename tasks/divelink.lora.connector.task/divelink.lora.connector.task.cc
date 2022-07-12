
#include "divelink.lora.connector.task.hpp"
#include <openedge/log.hpp>
#include <openedge/common/uart.hpp>
#include <support/device/systembase/ulory.hpp>

//static component instance that has only single instance
static divelink_lora_connector_task* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new divelink_lora_connector_task(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

//static instance
static device::systembase::ulory* _ulory = nullptr;

divelink_lora_connector_task::divelink_lora_connector_task(){


}

divelink_lora_connector_task::~divelink_lora_connector_task(){

}

void divelink_lora_connector_task::execute(){
    if(_ulory){
        _ulory->help();
    }
    else {
        console::warn("SystemBase uLory device is not working...");
    }
}

void divelink_lora_connector_task::stop(){

}

bool divelink_lora_connector_task::configure(){

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
                    _ulory = new device::systembase::ulory(_device);
                    console::info("[{}] open device : {}({})", this->get_name(), port, baud);
                }
                else {
                    console::warn("SystemBase uLory bus interface is closed.");
                }
            }

            if(conf.contains("lora")){
                _sid = conf["lora"]["sid"].get<long>();
                _did = conf["lora"]["did"].get<long>();
                


            }
        }


    }
    catch(const json::exception& e){
            console::error("Profile read/access error : {}", e.what());
    }
    return true;
    
}

void divelink_lora_connector_task::cleanup(){
    if(_device){
        _device->close();
        delete _device;
    }

    if(_ulory){
        delete _ulory;
    }
}

void divelink_lora_connector_task::pause(){
    
}

void divelink_lora_connector_task::resume(){
    
}

void divelink_lora_connector_task::on_request(std::any data){

}
