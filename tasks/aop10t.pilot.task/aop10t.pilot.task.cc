

#include "aop10t.pilot.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <string>
#include <dlfcn.h>
#include <openedge/util/validation.hpp>
#include <services/lsis.fenet.connector.service/fenet.connector.service.hpp>

using namespace std;

//task create & release
static aop10tPilotTask* _instance = nullptr;
core::task::runnable* create(){
    if(!_instance)
        _instance = new aop10tPilotTask();
    return _instance;
}

void release(){
    if(_instance){
        delete _instance;
        _instance = nullptr;
    }
}

bool aop10tPilotTask::configure(){

    //1. load services required
    vector<string> svclist = getServices();
    for(string& svcname: svclist){
        if(!load(svcname))
            spdlog::warn("{} load failed", svcname);
    }

    //2. check for service validation

    return true;
}

void aop10tPilotTask::execute(){
    //1. read data from PLC
    fenetConnectorService* fenet { nullptr };
    if(!serviceContainer["lsis.fenet.connector.service"].ptrService){
        fenet = dynamic_cast<fenetConnectorService*>(serviceContainer["lsis.fenet.connector.service"].ptrService);
        //1.1 request data
        if(fenet){
            fenet->read("%MW0", 2);
        }
        
    }

    //2. store the data into mongoDB
}

void aop10tPilotTask::cleanup(){
    unload();
}


bool aop10tPilotTask::load(const string& svcname){
    spdlog::info(" * Load dependant service : {}", svcname);
    string path = "./"+svcname;
    if(!exist(path.c_str())){
        spdlog::error("{} does not exist", svcname);
        return false;
    }

    serviceContainer[svcname].handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if(!serviceContainer[svcname].handle){
        spdlog::error("{} cannot load", svcname);
    }
    else{
        create_service pfCreate = (create_service)dlsym(serviceContainer[svcname].handle, "createService");
        if(!pfCreate){
            spdlog::error("{} access failed", svcname);
        }
        else{
            serviceContainer[svcname].ptrService = pfCreate();
            return serviceContainer[svcname].ptrService->initService();
        }
    }

    dlclose(serviceContainer[svcname].handle);
    serviceContainer.erase(svcname);
    return false;
}

void aop10tPilotTask::unload(){
    //release all services installed
    for(auto svc:serviceContainer){
        release_service pfService = (release_service)dlsym(svc.second.handle, "releaseService");
        if(pfService) pfService();
        dlclose(svc.second.handle);
    }
}