

#include "aop10t.pilot.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <string>
#include <dlfcn.h>
#include <openedge/util/validation.hpp>
#include <services/lsis.fenet.connector.service/lsis.fenet.connector.api.hpp>
#include <services/mongodb.connector.service/mongodb.connector.api.hpp>
#include <services/mqtt.publisher.service/mqtt.publisher.api.hpp>
#include <3rdparty/jsonrpccxx/client.hpp>
#include <algorithm>
#include <stdexcept>
using namespace std;

//static component instance that has only single instance
static aop10tPilotTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new aop10tPilotTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

const char* svc_fenet = "lsis.fenet.connector.service";
const char* svc_mongo = "mongodb.connector.service";
const char* svc_mqtt = "mqtt.publisher.service";

bool aop10tPilotTask::configure(){

    _logPerf.open("aop10tPilotTask.perf.txt");

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

    if(std::find(svclist.begin(), svclist.end(), svc_mqtt)==svclist.end()){
        spdlog::error("<{}> service must be required.", svc_mqtt);
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

    if(_serviceHandles.find(svc_mqtt)!=_serviceHandles.end()){
        _mqttConnector = make_unique<core::task::localServiceConnector>(_serviceHandles[svc_mqtt].ptrService->getServicePort());
        _mqttPublisher = make_shared<jsonrpccxx::JsonRpcClient>(*_mqttConnector.get(), jsonrpccxx::version::v2);
        _mqttServiceAPI = make_unique<mqttServiceAPI>(*_mqttPublisher.get());
    }

    return true;
}

void aop10tPilotTask::execute(){
    auto t_now = std::chrono::high_resolution_clock::now();
    //connection
    try {
        serviceHandle& _fenetHandle = _serviceHandles[svc_fenet]; //LSIS FEnet Service
        serviceHandle& _mongodbHandle = _serviceHandles[svc_mongo];; //MongoDB Service
        serviceHandle& _mqttHandle = _serviceHandles[svc_mqtt];; //MQTT Service

        if(_fenetHandle.ptrService){
            json block = json::parse(getProfile()->getCustom("block"));

            if(block.find("address")!=block.end() && block.find("size")!=block.end()){
                string address = block["address"].get<string>();
                int size = block["size"].get<int>();
                vector<uint8_t> rawdata = _fenetServiceAPI->read_block(address, size);

                switch(address.at(2)){
                    case 'W':
                    break;
                    case 'D': //double word
                        {
                            spdlog::info("Received raw data size {}", rawdata.size());
                            string data;
                            for(uint8_t d:rawdata)
                                data.append(fmt::format("{:x} ", d));
                            spdlog::info("Data : {}", data);
                        }
                    break;
                }

                //telegraf interface formatting
        //         string data;
        //         string msg = fmt::format("Data,host={} 0_0={:d},0_1={:d},0_2={:d},0_3={:d},0_4={:d},0_5={:d},0_6={:d},0_7={:d},0_8={:d},0_9={:d},0_10={:d},0_11={:d},0_12={:d},0_13={:d},0_14={:d},0_15={:d},5={:03.2f},6={:03.2f},7={:03.2f},8={:03.2f},9={:03.2f},10={:03.2f},11={:03.2f},12={:03.2f},13={:03.2f},14={:03.2f},15={:03.2f},16={:03.2f}"
		// 		,_mqtt_hostname,
		// 		(int)registers[0]&&0x0001,(int)registers[0]&&0x0002,(int)registers[0]&&0x0004,(int)registers[0]&&0x0008,
		// 		(int)registers[0]&0x0010,(int)registers[0]&&0x0020,(int)registers[0]&&0x0040,(int)registers[0]&&0x0080,
		// 		(int)registers[0]&&0x0100,(int)registers[0]&&0x0200,(int)registers[0]&&0x0400,(int)registers[0]&&0x0800,
		// 		(int)registers[0]&&0x1000,(int)registers[0]&&0x2000,(int)registers[0]&&0x4000,(int)registers[0]&&0x8000,
		// 		registers[5]/20,registers[6]/20,registers[7]/20,registers[8]/20,registers[9],registers[10]/1600,
		// 		registers[11]/1600,registers[12]/1600,registers[13]/1600,registers[14],registers[15]/16000*15,registers[16]/160);
		// console->info("{}",msg);

                if(_mqttHandle.ptrService){
                    _mqttServiceAPI->publish("test");
                    spdlog::info("Published Data");
                }
            }
        }

        // code here to parse the raw data
        // if(_mongodbHandle.ptrService){
        //     json test;
        //     test["test"] = 1;
        //     _mongoServiceAPI->insert(test.dump());
        // }

    } 
    catch (jsonrpccxx::JsonRpcException &e) {
        spdlog::warn("RPC Error : {}", e.what());
    }

    auto t_elapsed = std::chrono::high_resolution_clock::now();
    if(_logPerf.is_open()){
        _logPerf << std::chrono::duration<double, std::chrono::seconds::period>(t_elapsed - t_now).count() << endl;
    }
}

void aop10tPilotTask::cleanup(){
    _logPerf.close();

    serviceHandle& _fenetHandle = _serviceHandles[svc_fenet]; //LSIS FEnet Service
    serviceHandle& _mongodbHandle = _serviceHandles[svc_mongo]; //LSIS MongoDB Service

    if(_fenetHandle.handle){
        release_service pfRelease = (release_service)dlsym(_fenetHandle.handle, "release");
        if(pfRelease)
            pfRelease();
        dlclose(_fenetHandle.handle);
        _fenetHandle.handle = nullptr;
    }

    if(_mongodbHandle.handle){
        release_service pfRelease = (release_service)dlsym(_mongodbHandle.handle, "release");
        if(pfRelease)
            pfRelease();
        dlclose(_mongodbHandle.handle);
        _mongodbHandle.handle = nullptr;
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