

#include "aop10t.pilot.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <string>
#include <dlfcn.h>
#include <openedge/util/validation.hpp>
#include <openedge/core/service.hpp>
#include <openedge/core/bus.hpp>
#include <3rdparty/jsonrpccxx/client.hpp>
#include <jsonrpccxx/iclientconnector.hpp>
#include <jsonrpccxx/server.hpp>

class fenetServiceAPI {
public:
  explicit fenetServiceAPI(JsonRpcClient &client) : client(client) {}
  bool test(const int& value) { return client.CallMethod<bool>(1, "test", {value}); }
private:
  JsonRpcClient& client;
};

// class InMemoryConnector : public jsonrpccxx::IClientConnector {
// public:
//   explicit InMemoryConnector(jsonrpccxx::JsonRpcServer &server) : server(server) {}
//   std::string Send(const std::string &request) override { return server.HandleRequest(request); }
// private:
//   jsonrpccxx::JsonRpcServer& server;
// };

using namespace std;

//static component instance that has only single instance
static aop10tPilotTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new aop10tPilotTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

bool aop10tPilotTask::configure(){

    //1. load service
    if(!this->_load_fenet_service())
        return false;

    if(!_fenetConnector.ptrService->initService(this->getProfile()->getServiceProfile("lsis.fenet.connector.service").c_str())){
        spdlog::error("FENet Connector initialization failed");
        return false;
    }

    _fenetConnector.ptrService

    return true;
}

void aop10tPilotTask::execute(){
    //connection
    spdlog::info("do aop10t pilot task execute");
    if(_fenetConnector.ptrService){
        spdlog::info("requesting to fenet service");

        /*
        auto jmsg = R"({"jsonrpc":"2.0","method": "test", "params":[1], "id":1})"_json;
        string response = _fenetConnector.ptrService->request(jmsg.dump());
        */
        

        //spdlog::info("RPC Response : {}", response);
    }
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

bool aop10tPilotTask::_load_mongo_service(){
    return false;
}