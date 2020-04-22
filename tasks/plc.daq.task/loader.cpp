

#include "plc.daq.task.hpp"
#include <openedge/core/service.hpp>
#include <dlfcn.h>
#include <string>

using namespace std;

bool plcDaqTask::loadService(const char* servicename, void* handle, core::iServiceBase* pImpl){

    string path = "./"+string(servicename);
    handle = dlopen(path.c_str(), RTLD_LAZY|RTLD_GLOBAL);
    if(handle){
        create_service pfcreate = (create_service)dlsym(handle, "create");
        if(!pfcreate){
            dlclose(handle);
            handle = nullptr;
            return false;
        }
        else{
            return std::move(pfcreate());
            pImpl = pfcreate();
            return true;
        }
    }

    return false;

}

void plcDaqTask::releaseService(){

    // for(auto service:_serviceContainer){
    //     release_service pfrelease = (release_service)dlsym(_service_handle, "release");
    //     service.second
    // }

}