

#include "aop10t.pilot.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <string>
#include <dlfcn.h>
#include <openedge/util/validation.hpp>
#include <services/lsis.fenet.connector.service/lsis.fenet.connector.api.hpp>
#include <services/mongodb.connector.service/mongodb.connector.api.hpp>
#include <3rdparty/jsonrpccxx/client.hpp>
#include <algorithm>
using namespace std;

//static component instance that has only single instance
static aop10tPilotTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new aop10tPilotTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

const char* svc_fenet = "lsis.fenet.connector.service";
const char* svc_mongo = "mongodb.connector.service";

bool aop10tPilotTask::configure(){
    //getting list of service
    vector<string> svclist = this->getProfile()->getRequiredServices();

    //required service check
    if(std::find(svclist.begin(), svclist.end(), svc_fenet)==svclist.end()){
        spdlog::error("<{}> service must be required.", svc_fenet);
        return false;
    }
        
    if(std::find(svclist.begin(), svclist.end(), svc_mongo)==svclist.end()){
        spdlog::error("<{}> service must be required.", svc_mongo);
        return false;
    }
        

    //load services
    for(string& svcname:svclist){
        _serviceHandles[svcname] = serviceHandle(svcname.c_str());
        if(!this->_load_service(_serviceHandles[svcname])){
            spdlog::error("Service<{}> load failed!", svcname);
            _serviceHandles.erase(svcname);
        }
    }

    //initialize all loaded services
    for(map<string, serviceHandle>::iterator itr = _serviceHandles.begin();itr!=_serviceHandles.end();++itr){
        if(!itr->second.ptrService->initService(this->getProfile()->getServiceProfile(itr->first.c_str()).c_str())){
            spdlog::error("Service<{}> initialization failed", itr->first);
        }
    }

    //find service handle and make required service connection
    if(_serviceHandles.find(svc_fenet)!=_serviceHandles.end()){
        _fenetConnector = make_unique<core::task::localServiceConnector>(_serviceHandles[svc_fenet].ptrService->getServicePort());
        _fenetAccessor = make_shared<jsonrpccxx::JsonRpcClient>(*_fenetConnector.get(), jsonrpccxx::version::v2);
        _fenetServiceAPI = make_unique<fenetServiceAPI>(*_fenetAccessor.get());
    }

    if(_serviceHandles.find(svc_mongo)!=_serviceHandles.end()){
        _mongoConnector = make_unique<core::task::localServiceConnector>(_serviceHandles[svc_mongo].ptrService->getServicePort());
        _mongoAccessor = make_shared<jsonrpccxx::JsonRpcClient>(*_mongoConnector.get(), jsonrpccxx::version::v2);
        _mongoServiceAPI = make_unique<mongoServiceAPI>(*_mongoAccessor.get());
    }

    return true;
}

void aop10tPilotTask::execute(){
    //connection
    try {
        serviceHandle& _fenetHandle = _serviceHandles[svc_fenet]; //LSIS FEnet Service
        serviceHandle& _mongodbHandle = _serviceHandles[svc_mongo];; //LSIS MongoDB Service

        if(_fenetHandle.ptrService){
            json action = json::parse(getProfile()->getCustom());

            if(action.find("address")!=action.end() && action.find("count"!=action.end())){
                _fenetServiceAPI->read_n(action["address"].get<string>(), action["count"].get<int>());
            }
        }

        // if(_mongodbHandle.ptrService){
        //     _mongoServiceAPI->test(1);
        // }
    } 
    catch (jsonrpccxx::JsonRpcException &e) {
        spdlog::warn("RPC Error : {}", e.what());
    }
}

void aop10tPilotTask::cleanup(){
    serviceHandle& _fenetHandle = _serviceHandles[svc_fenet]; //LSIS FEnet Service
    serviceHandle& _mongodbHandle = _serviceHandles[svc_mongo]; //LSIS MongoDB Service

    if(_fenetHandle.handle){
        release_service pfRelease = (release_service)dlsym(_fenetHandle.handle, "release");
        if(pfRelease)
            pfRelease();
        dlclose(_fenetHandle.handle);
    }

    if(_mongodbHandle.handle){
        release_service pfRelease = (release_service)dlsym(_mongodbHandle.handle, "release");
        if(pfRelease)
            pfRelease();
        dlclose(_mongodbHandle.handle);
    }

    spdlog::info("Cleanup the aop10tPilotTask");
}

bool aop10tPilotTask::_load_service(serviceHandle& service){
    string path = "./"+service.name;
    spdlog::info(" * Load service : {}", path);

    service.handle = dlopen(path.c_str(), RTLD_LAZY|RTLD_LOCAL);
    if(service.handle==nullptr)
        spdlog::error("{}",dlerror());
    assert(service.handle!=nullptr);
    
    create_service pfCreate = (create_service)dlsym(service.handle, "create");
    if(pfCreate){
        service.ptrService = pfCreate();
        return true;
    }
    else
        dlclose(service.handle);
    
    return false;
}