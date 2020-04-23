

#include "plc.daq.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <openedge/device/plc.hpp>
#include <dlfcn.h>
#include <string>
#include <services/plc.lsis.service/plc.lsis.service.hpp>

using namespace std;

//task create & release
static plcDaqTask* _instance = nullptr;
oe::core::rt_task::runnable* create(){
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

    //laod service file
    if(loadService("plc.lsis.service")){
        if(_plcServiceHandle){
            uint8_t (*pFunc)();
            pFunc = (uint8_t(*)())dlsym(_plcServiceHandle, "readByte");
            if(pFunc){
                spdlog::info("Read Byte : {0:x}", static_cast<unsigned char>(pFunc()));
            }
        }
        
    }

    return true;
}

void plcDaqTask::execute(){

    
}

void plcDaqTask::cleanup(){
    if(_plcServiceHandle){
        release_service pReleaseService = (release_service)dlsym(_plcServiceHandle, "releaseService");
        if(pReleaseService) pReleaseService();
        dlclose(_plcServiceHandle);
        _plcServiceHandle = nullptr;
    }
}

bool plcDaqTask::loadService(const char* servicename){
    spdlog::info("load {}", servicename);
    string path = "./"+string(servicename);
    _plcServiceHandle = dlopen(path.c_str(), RTLD_LAZY);
    if(_plcServiceHandle){
        create_service pCreateService = (create_service)dlsym(_plcServiceHandle, "createService");
        if(pCreateService) pCreateService();
        else { spdlog::error("Cannot open servie"); }
    }
    return true;
}

void plcDaqTask::unloadService(){

}