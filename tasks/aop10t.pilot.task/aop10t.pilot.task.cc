

#include "aop10t.pilot.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <string>
#include <dlfcn.h>
#include <openedge/util/validation.hpp>
#include <openedge/core/service.hpp>
#include <openedge/core/bus.hpp>
#include <3rdparty/jsonrpccxx/client.hpp>
#include <services/lsis.fenet.connector.service/lsis.fenet.connector.api.hpp>


using namespace std;

//static component instance that has only single instance
static aop10tPilotTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new aop10tPilotTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool aop10tPilotTask::configure(){

    //1. load service component
    if(!this->_load_fenet_service())
        return false;

    //2. connect with the loaded service
    spdlog::info("try to connect with the fenet service");
    _fenetConnector = make_unique<core::task::localServiceConnector>(*_fenetHandler.ptrService->getServicePort());
    jsonrpccxx::JsonRpcClient client(*_fenetConnector.get(), jsonrpccxx::version::v2);
    unique_ptr<fenetConnectorServiceAPI> api = make_unique<fenetConnectorServiceAPI>(client);

    if(!_fenetHandler.ptrService->initService(this->getProfile()->getServiceProfile("lsis.fenet.connector.service").c_str())){
        spdlog::error("FENet Connector initialization failed");
        return false;
    }

    api->test(1);

    return true;
}

void aop10tPilotTask::execute(){
    //connection
    if(_fenetHandler.ptrService){
        spdlog::info("requesting to fenet service");

        // auto jmsg = R"({"jsonrpc":"2.0","method": "request", "params":["0x123737377272737277"], "id":1})"_json;
        // string response = _fenetHandler.ptrService->request(jmsg.dump());
        // spdlog::info("RPC Response : {}", response);
    }
}

void aop10tPilotTask::cleanup(){
    if(_fenetHandler.handle){
        release_service pfRelease = (release_service)dlsym(_fenetHandler.handle, "release");
        if(pfRelease)
            pfRelease();
        dlclose(_fenetHandler.handle);
    }

    spdlog::info("Cleanup the aop10tPilotTask");
}

bool aop10tPilotTask::_load_fenet_service(){
    const char* svcname = "lsis.fenet.connector.service";
    string path = "./"+string(svcname);
    spdlog::info(" * Load dependant service : {}", path);

    _fenetHandler.handle = dlopen(path.c_str(), RTLD_LAZY|RTLD_LOCAL);
    if(_fenetHandler.handle==nullptr)
        spdlog::error("{}",dlerror());
    assert(_fenetHandler.handle!=nullptr);
    
    create_service pfCreate = (create_service)dlsym(_fenetHandler.handle, "create");
    if(pfCreate){
        _fenetHandler.ptrService = pfCreate();
        return true;
    }
    else
        dlclose(_fenetHandler.handle);
    
    return false;
}

bool aop10tPilotTask::_load_mongo_service(){
    return false;
}