
#include "divelink.m64.connector.task.hpp"
#include <openedge/log.hpp>
#include <openedge/common/uart.hpp>

//static component instance that has only single instance
static divelink_m64_connector_task* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new divelink_m64_connector_task(); return _instance; }
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

    if(_device)
        _device->close();

    json config = json::parse(get_profile()->get("configuration"));
    if(config.find("serial")!=config.end()){
        
        if(config["serial"].find("device")!=config["serial"].end() && config["serial"].find("baudrate")!=config["serial"].end()){
            string dev = config["serial"]["device"].get<string>();
            unsigned int baud = config["serial"]["baudrate"].get<unsigned int>();

            console::info("> Open Device for m64 : {}({})", dev, baud);
            
            _device = new bus::sync_uart(dev.c_str(), baud);

            if(config["serial"].find("timeout")!=config["serial"].end()){
                _t_out = config["serial"]["timeout"].get<double>();
                console::info("> Set read timeout : {}sec.", _t_out);
            }

            if(!_device->open()){
                console::error("Device {} cannnot open. Check the device.", dev);
                return false;
            }
        }
        else {
            console::error("Wrong configuration for the device. Device port and bauadrate are required."); 
        }
    }
    return true;
    
}

void divelink_m64_connector_task::cleanup(){
    if(_device){
        _device->close();
        delete _device;
    }
}

void divelink_m64_connector_task::pause(){
    
}

void divelink_m64_connector_task::resume(){
    
}

void divelink_m64_connector_task::on_request(std::any data){

}
