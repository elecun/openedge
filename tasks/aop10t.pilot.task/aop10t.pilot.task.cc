

#include "aop10t.pilot.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <string>
#include <dlfcn.h>
#include <openedge/util/validation.hpp>
#include <openedge/core/service.hpp>
#include <openedge/core/bus.hpp>

using namespace std;

//core::bus::iDeviceBusTCP* _fenet { nullptr };

//task create & release
static aop10tPilotTask* _instance = nullptr;
oe::core::task::runnable* create(){
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
    vector<string> svclist = getRequiredServices();
    for(string& svcname: svclist){
        if(!_load_service(svcname)){
            spdlog::error("{} cannot be loaded", svcname);
        }
    }

    // //2. get service
    if(serviceContainer["lsis.fenet.connector.service"].ptrService){
        //_fenet = dynamic_cast<core::bus::iDeviceBusTCP*>(serviceContainer["lsis.fenet.connector.service"].ptrService);
        string address = this->getProfile()->data["services"]["lsis.fenet.connector.service"]["connection"]["address"].get<string>();
        int port = this->getProfile()->data["services"]["lsis.fenet.connector.service"]["connection"]["port"].get<int>();

        spdlog::info("----------");
        spdlog::info(" + FEnet Connection : {}({})", address, port);
        spdlog::info("----------");

        // if(_fenet->connect(address.c_str(), port))
        //     spdlog::info("FEnet Connected");
        // else spdlog::warn("Cannot connect to the FEnet server");
    }
    
    // if(!serviceContainer["mongodb.connector.service"].ptrService)
    //     _db = dynamic_cast<mongodbConnectorService*>(serviceContainer["mongodb.connector.service"].ptrService);

    return true;
}

void aop10tPilotTask::execute(){
    //1. request 
    // if(_fenet)
    //     _fenet->request("%MW0", 10); //request to read data (async)

    //2. store the data into mongoDB
}

void aop10tPilotTask::cleanup(){
    // if(_fenet){
    //     _fenet->disconnect();
    // }
        

    // _unload_service();
}


bool aop10tPilotTask::_load_service(const string& svcname){
    spdlog::info(" * Load dependant service : {}", svcname);
    string path = "./"+svcname;
    if(!exist(path.c_str())){
        spdlog::error("{} does not exist", svcname);
        return false;
    }

    serviceContainer[svcname].handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
    assert(serviceContainer[svcname].handle!=nullptr);
    if(serviceContainer[svcname].handle){
        create_service pfCreate = (create_service)dlsym(serviceContainer[svcname].handle, "createService");
        spdlog::info("create service instance");
        if(!pfCreate){
            spdlog::error("{} access failed", svcname);
        }
        else{
            spdlog::info("call service instance");
            serviceContainer[svcname].ptrService = pfCreate();
            return serviceContainer[svcname].ptrService->initService(); //call initialize
        }
        dlclose(serviceContainer[svcname].handle);
    }
    
    serviceContainer.erase(svcname);
    return false;
}

void aop10tPilotTask::_unload_service(){
    //release all services installed
    for(auto svc:serviceContainer){
        release_service pfService = (release_service)dlsym(svc.second.handle, "releaseService");
        if(pfService) pfService();
        dlclose(svc.second.handle);
    }
}