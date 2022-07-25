
#include "device.remotedio.modbus.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static device_remotedio_modbus* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new device_remotedio_modbus(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void device_remotedio_modbus::execute(){
    if(_modbus){
        /* publish the DI & DO Values (publish if changed) */
    }
 
}

void device_remotedio_modbus::stop(){

}

bool device_remotedio_modbus::configure(){
    try {

        const json& profile = this->get_profile()->raw();
        if(profile.contains(PROFILE_CONFIGURATIONS_KEY)){
            json conf = profile[PROFILE_CONFIGURATIONS_KEY];
        }

    }
    catch(const json::exception& e){
        console::error("Error : {}", e.what());
    }
    return true;
}

void device_remotedio_modbus::cleanup(){

}

void device_remotedio_modbus::pause(){
    
}

void device_remotedio_modbus::resume(){
    
}

void device_remotedio_modbus::on_connect(int rc){

}
void device_remotedio_modbus::on_disconnect(int rc){

}
void device_remotedio_modbus::on_publish(int mid){

}
void device_remotedio_modbus::on_message(const struct mosquitto_message* message){

}
void device_remotedio_modbus::on_subscribe(int mid, int qos_count, const int* granted_qos){

}
void device_remotedio_modbus::on_unsubscribe(int mid){

}
void device_remotedio_modbus::on_log(int level, const char* str){

}
void device_remotedio_modbus::on_error(){
    
}

