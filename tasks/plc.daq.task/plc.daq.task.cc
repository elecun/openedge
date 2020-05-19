

#include "plc.daq.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <dlfcn.h>
#include <string>
#include <vector>
#include <openedge/core/device.hpp>
#include <openedge/core/bus.hpp>
#include <openedge/util/validation.hpp>
#include <openedge/core/protocol.hpp>
#include <openedge/core/profile.hpp>


using namespace std;

//task create & release
static plcDaqTask* _instance = nullptr;
core::task::runnable* create(){
    if(!_instance)
        _instance = new plcDaqTask();
    return _instance;
}

void release(){
    if(_instance){
        delete _instance;
        _instance = nullptr;
    }
}

static core::iDevicePLC* _plc = nullptr;
static core::bus::iDeviceBusTCP* _bus = nullptr;
static core::iProtocolRaw* _protocol = nullptr;

bool plcDaqTask::configure(){

    vector<string> services = this->getProfile()->data["services"]["required"];

    //read task configurations
    for(string& service:services){
        if(!loadService(service.c_str()))
            spdlog::warn("{} cannot be loaded", service.c_str());
        else
            spdlog::info(" + Loaded service : {}", service);
    }

    auto itrPlc = serviceContainer.find("plc.lsis.service");
    if(itrPlc!=serviceContainer.end())
        _plc = dynamic_cast<core::iDevicePLC*>(itrPlc->second.ptrService);

    auto itrBus = serviceContainer.find("bus.tcp.service");
    if(itrBus!=serviceContainer.end())
        _bus = dynamic_cast<core::bus::iDeviceBusTCP*>(itrBus->second.ptrService);

    auto itrProtocol = serviceContainer.find("xgt.protocol.service");
    if(itrProtocol!=serviceContainer.end())
        _protocol = dynamic_cast<core::iProtocolRaw*>(itrProtocol->second.ptrService);

    return true;
}

void plcDaqTask::execute(){
    if(_plc && _bus){
        uint8_t value = _plc->readByte(_bus, _protocol, "%MW100");
        
        spdlog::info("Read Byte : 0x{0:x}", static_cast<unsigned char>(value));
    }
}

void plcDaqTask::cleanup(){
    spdlog::info("Cleanup <{}>", this->_taskname);
    unloadService();
    serviceContainer.clear();
}

bool plcDaqTask::loadService(const char* servicename){

    string path = "./"+string(servicename);
    if(!exist(path.c_str())){
        spdlog::error("{} does not exist", servicename);
        return false;
    }
    
    serviceContainer[servicename].handle = dlopen(path.c_str(),RTLD_LAZY | RTLD_LOCAL);
    if(serviceContainer[servicename].handle==nullptr){
        spdlog::error("{} cannot load", servicename);
    }
    else{
        create_service pfCreate = (create_service)dlsym(serviceContainer[servicename].handle, "createService");
        if(!pfCreate){
            spdlog::error("{} access failed", servicename);
        }
        else{
            serviceContainer[servicename].ptrService = pfCreate();
            return serviceContainer[servicename].ptrService->initService();
        }
    }

    dlclose(serviceContainer[servicename].handle);
    serviceContainer.erase(servicename);
    return false;
}

void plcDaqTask::unloadService(){
    for(auto service:serviceContainer){
        release_service pfService = (release_service)dlsym(service.second.handle, "releaseService");
        if(pfService) pfService();
        dlclose(service.second.handle);
    }
}