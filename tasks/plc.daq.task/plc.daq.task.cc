

#include "plc.daq.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <dlfcn.h>
#include <string>
#include <vector>
#include <openedge/core/device.hpp>
#include <openedge/core/bus.hpp>


using namespace std;

//task create & release
static plcDaqTask* _instance = nullptr;
core::rt_task::runnable* create(){
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

plcDaqTask::~plcDaqTask(){

}

bool plcDaqTask::configure(){

    //read task configurations
    vector<string> services { "plc.lsis.service", "bus.tcp.service"};
    for(string& service:services){
        if(!loadService(service.c_str()))
            spdlog::warn("{} cannot be loaded", service.c_str());
    }

    auto itrPlc = _srvContainer.find("plc.lsis.service");
    core::iDevicePLC* _plc = nullptr;
    if(itrPlc!=_srvContainer.end())
        _plc = dynamic_cast<core::iDevicePLC*>(itrPlc->second);

    auto itrBus = _srvContainer.find("bus.tcp.service");
    core::bus::iDeviceBusTCP* _bus = nullptr;
    if(itrBus!=_srvContainer.end())
        _bus = dynamic_cast<core::bus::iDeviceBusTCP*>(itrBus->second);

    if(_plc && _bus)
        _plc->configBus(_bus);
    else
        spdlog::info("cannot cast the service");

    return true;
}

void plcDaqTask::execute(){
    auto itr = _srvContainer.find("plc.lsis.service");
    if(itr!=_srvContainer.end()){
        if(itr->second){
            core::iDevicePLC* _plc = dynamic_cast<core::iDevicePLC*>(itr->second);
            spdlog::info("Read Byte : 0x{0:x}", static_cast<unsigned char>(_plc->readByte("%MW100")));
        }
    }
    
}

void plcDaqTask::cleanup(){

    for(auto service:_srvHandleContainer){
        release_service fService = (release_service)dlsym(service.second, "releaseService");
        if(fService) fService();
        dlclose(service.second);
        spdlog::info("plcDaqTask cleanup");
    }

    _srvHandleContainer.clear();
    _srvContainer.clear();

    // if(_plcServiceHandle){
    //     release_service pReleaseService = (release_service)dlsym(_plcServiceHandle, "releaseService");
    //     if(pReleaseService) pReleaseService();
    //     dlclose(_plcServiceHandle);
    //     _plcServiceHandle = nullptr;
    // }
}

bool plcDaqTask::loadService(const char* servicename){

    string path = "./"+string(servicename);
    _srvHandleContainer.insert(std::pair<string, void*>(servicename, dlopen(path.c_str(),RTLD_LAZY | RTLD_LOCAL)));
    if(_srvHandleContainer[servicename]){
        create_service fService = (create_service)dlsym(_srvHandleContainer[servicename], "createService");
        if(fService){
            _srvContainer.insert(std::pair<string, oe::core::iService*>(servicename, fService()));
            return true;
        }
    }

    dlclose(_srvHandleContainer[servicename]);
    _srvHandleContainer.erase(servicename);
    return false;

    //backup
    // [servicename] = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
    // _serviceHandleContainer[servicename]

    // if(_plcServiceHandle){
    //     create_service pCreateService = (create_service)dlsym(_plcServiceHandle, "createService");
    //     if(pCreateService) pCreateService();
    //     else { spdlog::error("Cannot open servie"); }
    // }
    return true;
}

void plcDaqTask::unloadService(){

}