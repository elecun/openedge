

#include "aop10t.pilot.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <string>
#include <dlfcn.h>
#include <openedge/util/validation.hpp>
#include <services/lsis.fenet.connector.service/fenet.connector.service.hpp>
//#include <services/mongodb.connector.service/mongodb.connector.hpp>

using namespace std;

//fenet
fenetConnectorService* _fenet { nullptr };
//mongodbConnectorService* _db { nullptr };

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

int aop10tPilotTask::readf(vector<byte>& packet){

}

bool aop10tPilotTask::configure(){

    //1. load services required
    vector<string> svclist = getServices();
    for(string& svcname: svclist){
        if(!load(svcname))
            spdlog::warn("{} load failed", svcname);
    }

    //2. check for service validation
    //code here

    //3. get service
    if(!serviceContainer["lsis.fenet.connector.service"].ptrService){
        _fenet = dynamic_cast<fenetConnectorService*>(serviceContainer["lsis.fenet.connector.service"].ptrService);
        _fenet->setRcvTimeout(1);
        if(_fenet->connect("192.168.100.6", 27017)){
            _fenet->setReadCallback(aop10tPilotTask::readf);
        }
    }
    
    // if(!serviceContainer["mongodb.connector.service"].ptrService)
    //     _db = dynamic_cast<mongodbConnectorService*>(serviceContainer["mongodb.connector.service"].ptrService);

    return true;
}

void aop10tPilotTask::execute(){
    //1. request 
    if(_fenet)
        _fenet->request("%MW0", 10); //request to read data (async)

    //2. store the data into mongoDB
}

void aop10tPilotTask::cleanup(){
    if(_fenet)
        _fenet = nullptr;

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
            return serviceContainer[svcname].ptrService->initService(); //call initialize
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