

#include "aop10t.pilot.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <string>
#include <dlfcn.h>
#include <openedge/util/validation.hpp>
#include <services/lsis.fenet.connector.service/lsis.fenet.connector.api.hpp>
#include <services/mongodb.connector.service/mongodb.connector.api.hpp>
#include <3rdparty/jsonrpccxx/client.hpp>

using namespace std;

//static component instance that has only single instance
static aop10tPilotTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new aop10tPilotTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool aop10tPilotTask::configure(){

    if(!this->_load_fenet_service())  return false; //load fenet service component file
    if(!this->_load_mongo_service())  return false; //load mongo service component file

    if(!_fenetHandle.ptrService->initService(this->getProfile()->getServiceProfile("lsis.fenet.connector.service").c_str())){
        spdlog::error("FENet Connector initialization failed");
        return false;
    }

    if(!_mongoHandle.ptrService->initService(this->getProfile()->getServiceProfile("mongodb.connector.service").c_str())){
        spdlog::error("MongoDB Connector initialization failed");
        return false;
    }

    //2. connect with the loaded service
    _fenetConnector = make_unique<core::task::localServiceConnector>(_fenetHandle.ptrService->getServicePort());
    _fenetAccessor = make_shared<jsonrpccxx::JsonRpcClient>(*_fenetConnector.get(), jsonrpccxx::version::v2);
    _fenetServiceAPI = make_unique<fenetServiceAPI>(*_fenetAccessor.get());


    _mongoConnector = make_unique<core::task::localServiceConnector>(_mongoHandle.ptrService->getServicePort());
    _mongoAccessor = make_shared<jsonrpccxx::JsonRpcClient>(*_mongoConnector.get(), jsonrpccxx::version::v2);
    _mongoServiceAPI = make_unique<mongoServiceAPI>(*_mongoAccessor.get());

    return true;
}

void aop10tPilotTask::execute(){
    //connection
    try {

        if(_fenetHandle.ptrService){
            _fenetServiceAPI->write("123");
        }

        if(_mongoHandle.ptrService){
            _mongoServiceAPI->test(1);
        }
    } 
    catch (jsonrpccxx::JsonRpcException &e) {
        spdlog::warn("RPC Error : {}", e.what());
    }
}

void aop10tPilotTask::cleanup(){
    if(_fenetHandle.handle){
        release_service pfRelease = (release_service)dlsym(_fenetHandle.handle, "release");
        if(pfRelease)
            pfRelease();
        dlclose(_fenetHandle.handle);
    }

    if(_mongoHandle.handle){
        release_service pfRelease = (release_service)dlsym(_mongoHandle.handle, "release");
        if(pfRelease)
            pfRelease();
        dlclose(_mongoHandle.handle);
    }

    spdlog::info("Cleanup the aop10tPilotTask");
}

bool aop10tPilotTask::_load_fenet_service(){
    const char* svcname = "lsis.fenet.connector.service";
    string path = "./"+string(svcname);
    spdlog::info(" * Load dependant service : {}", path);

    _fenetHandle.handle = dlopen(path.c_str(), RTLD_LAZY|RTLD_LOCAL);
    if(_fenetHandle.handle==nullptr)
        spdlog::error("{}",dlerror());
    assert(_fenetHandle.handle!=nullptr);
    
    create_service pfCreate = (create_service)dlsym(_fenetHandle.handle, "create");
    if(pfCreate){
        _fenetHandle.ptrService = pfCreate();
        return true;
    }
    else
        dlclose(_fenetHandle.handle);
    
    return false;
}

bool aop10tPilotTask::_load_mongo_service(){
    const char* svcname = "mongodb.connector.service";
    string path = "./"+string(svcname);
    spdlog::info(" * Load dependant service : {}", path);

    _mongoHandle.handle = dlopen(path.c_str(), RTLD_LAZY|RTLD_LOCAL);
    if(_mongoHandle.handle==nullptr)
        spdlog::error("{}",dlerror());
    assert(_mongoHandle.handle!=nullptr);
    
    create_service pfCreate = (create_service)dlsym(_mongoHandle.handle, "create");
    if(pfCreate){
        _mongoHandle.ptrService = pfCreate();
        return true;
    }
    else
        dlclose(_mongoHandle.handle);
    
    return false;
}