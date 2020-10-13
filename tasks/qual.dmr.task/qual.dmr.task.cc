

#include "qual.dmr.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <dlfcn.h>
#include <openedge/util/validation.hpp>
#include <services/modbus.rtu.service/modbus.rtu.api.hpp>
#include <3rdparty/jsonrpccxx/client.hpp>
#include <algorithm>
#include <stdexcept>
using namespace std;

//static component instance that has only single instance
static qualDmrTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new qualDmrTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

const char* svc_modbus = "modbus.rtu.service";

bool qualDmrTask::configure(){

    //getting list of service층
    vector<string> svclist = this->getProfile()->getRequiredServices();

    //required service check
    if(std::find(svclist.begin(), svclist.end(), svc_modbus)==svclist.end()){
        spdlog::error("<{}> service must be required.", svc_modbus);
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
        if(!itr->second.pService->initService(this->getProfile()->getServiceProfile(itr->first.c_str()).c_str())){
            spdlog::error("Service<{}> initialization failed", itr->first);
        }
    }

    //find service handle and make required service connection
    if(_serviceHandles.find(svc_modbus)!=_serviceHandles.end()){
        _modbusRtuConnector = make_unique<core::task::localServiceConnector>(_serviceHandles[svc_modbus].pService->getServicePort());
        _modbusRtuAccessor = make_shared<jsonrpccxx::JsonRpcClient>(*_modbusRtuConnector.get(), jsonrpccxx::version::v2);
        _modbusRtuServiceAPI = make_unique<modbusRtuServiceAPI>(*_modbusRtuAccessor.get());
    }

    //configure a task activity
    if(_serviceHandles[svc_modbus].pService){
        json activity = json::parse(getProfile()->get("activity"));
        json rconfig = activity["read_holding_registers"];
        json wconfig = activity["write_holding_registers"];

        if(rconfig.find("address")!=rconfig.end()){
            _read_address = rconfig["address"].get<uint16_t>();
        }

        if(wconfig.find("address")!=wconfig.end()){
            _write_address = wconfig["address"].get<uint16_t>();
        }
    }


    return true;
}

void qualDmrTask::execute(){
    auto t_now = std::chrono::high_resolution_clock::now();
    static bool onoff = true;
    vector<uint16_t> on = {0x0001};
    vector<uint16_t> off = {0x0000};

    //connection
    try {
        serviceHandle& _modbusHandle = _serviceHandles[svc_modbus]; //modbus RTU Service

        if(_modbusHandle.pService){
            
            
            if(onoff){ //on
                _modbusRtuServiceAPI->write_holding_registers(_write_address, on);
                onoff = false;
            }
            else {
                _modbusRtuServiceAPI->write_holding_registers(_write_address, off);
                onoff = true;
            }
        }
    } 
    catch (jsonrpccxx::JsonRpcException &e) {
        spdlog::warn("RPC Error : {}", e.what());
    }
}

void qualDmrTask::cleanup(){

    serviceHandle& _modbusHandle = _serviceHandles[svc_modbus]; //modbus RTU Service

    if(_modbusHandle.pfHandle){
        release_service pfRelease = (release_service)dlsym(_modbusHandle.pfHandle, "release");
        if(pfRelease)
            pfRelease();
        dlclose(_modbusHandle.pfHandle);
        _modbusHandle.pfHandle = nullptr;
    }

    spdlog::info("Cleanup the qualDmrTask");
}

bool qualDmrTask::_load_service(serviceHandle& service){
    
    string path = "./"+service.getName();
    spdlog::info(" * Load service : {}", path);

    service.pfHandle = dlopen(path.c_str(), RTLD_LAZY|RTLD_LOCAL);
    if(service.pfHandle==nullptr)
        spdlog::error("{}",dlerror());
    assert(service.pfHandle!=nullptr);
    
    create_service pfCreate = (create_service)dlsym(service.pfHandle, "create");
    if(pfCreate){
        service.pService = pfCreate();
        return true;
    }
    else
        dlclose(service.pfHandle);
    
    return false;
}