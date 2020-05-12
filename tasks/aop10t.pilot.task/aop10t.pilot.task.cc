

#include "aop10t.pilot.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <string>
#include <dlfcn.h>
#include <openedge/util/validation.hpp>
#include <openedge/core/service.hpp>
#include <openedge/core/bus.hpp>

using namespace std;

//static component instance that has only single instance
static aop10tPilotTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new aop10tPilotTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool aop10tPilotTask::configure(){
    //getting required services from task profile, and then load each service on service container.
    // vector<string> svclist = this->getProfile()->getRequiredServices();
    // for(string& svcname: svclist){
    //     if(!_load_service(svcname))
    //         spdlog::error("{} load failed", svcname);
    // }

    //1. load service
    if(!this->_load_fenet_service())
        return false;

    _fenetConnector.ptrService->initService();

    if(serviceContainer["lsis.fenet.connector.service"].ptrService){
        //_fenet = dynamic_cast<core::bus::iDeviceBusTCP*>(serviceContainer["lsis.fenet.connector.service"].ptrService);
        //string address = this->getProfile()->data["services"]["lsis.fenet.connector.service"]["connection"]["address"].get<string>();
        //int port = this->getProfile()->data["services"]["lsis.fenet.connector.service"]["connection"]["port"].get<int>();

        // spdlog::info("----------");
        // spdlog::info(" + FEnet Connection : {}({})", address, port);
        // spdlog::info("----------");

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
    if(_fenetConnector.handle){
        release_service pfRelease = (release_service)dlsym(_fenetConnector.handle, "release");
        if(pfRelease)
            pfRelease();
        dlclose(_fenetConnector.handle);
    }

    spdlog::info("Cleanup the aop10tPilotTask");
}

bool aop10tPilotTask::_load_fenet_service(){
    const char* svcname = "lsis.fenet.connector.service";
    string path = "./"+string(svcname);
    spdlog::info(" * Load dependant service : {}", path);

    _fenetConnector.handle = dlopen(path.c_str(), RTLD_LAZY|RTLD_LOCAL);
    if(_fenetConnector.handle==nullptr)
        spdlog::error("{}",dlerror());
    assert(_fenetConnector.handle!=nullptr);
    
    create_service pfCreate = (create_service)dlsym(_fenetConnector.handle, "create");
    if(pfCreate){
        _fenetConnector.ptrService = pfCreate();
        return true;
    }
    else
        dlclose(_fenetConnector.handle);
    
    return false;
}
