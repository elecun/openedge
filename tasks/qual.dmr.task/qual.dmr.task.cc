

#include "qual.dmr.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <dlfcn.h>
#include <openedge/util/validation.hpp>
#include <services/lsis.fenet.connector.service/lsis.fenet.connector.api.hpp>
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
    if(std::find(svclist.begin(), svclist.end(), svc_fenet)==svclist.end()){
        spdlog::error("<{}> service must be required.", svc_fenet);
        return false;
    }
        
    // if(std::find(svclist.begin(), svclist.end(), svc_mongo)==svclist.end()){
    //     spdlog::error("<{}> service must be required.", svc_mongo);
    //     return false;
    // }

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
        if(!itr->second.pService->initService(this->getProfile()->getServiceProfile(itr->first.c_str()).c_str())){
            spdlog::error("Service<{}> initialization failed", itr->first);
        }
    }

    //find service handle and make required service connection
    if(_serviceHandles.find(svc_fenet)!=_serviceHandles.end()){
        _fenetConnector = make_unique<core::task::localServiceConnector>(_serviceHandles[svc_fenet].pService->getServicePort());
        _fenetAccessor = make_shared<jsonrpccxx::JsonRpcClient>(*_fenetConnector.get(), jsonrpccxx::version::v2);
        _fenetServiceAPI = make_unique<fenetServiceAPI>(*_fenetAccessor.get());
    }

    // if(_serviceHandles.find(svc_mongo)!=_serviceHandles.end()){
    //     _mongoConnector = make_unique<core::task::localServiceConnector>(_serviceHandles[svc_mongo].pService->getServicePort());
    //     _mongoAccessor = make_shared<jsonrpccxx::JsonRpcClient>(*_mongoConnector.get(), jsonrpccxx::version::v2);
    //     _mongoServiceAPI = make_unique<mongoServiceAPI>(*_mongoAccessor.get());
    // }

    if(_serviceHandles.find(svc_mqtt)!=_serviceHandles.end()){
        _mqttConnector = make_unique<core::task::localServiceConnector>(_serviceHandles[svc_mqtt].pService->getServicePort());
        _mqttPublisher = make_shared<jsonrpccxx::JsonRpcClient>(*_mqttConnector.get(), jsonrpccxx::version::v2);
        _mqttServiceAPI = make_unique<mqttServiceAPI>(*_mqttPublisher.get());
    }

    //configure a task activity
    if(_serviceHandles[svc_fenet].pService){
        json activity = json::parse(getProfile()->get("activity"));
        json block_config = activity["read_block"];

        if(block_config.find("address")!=block_config.end() && block_config.find("bytes")!=block_config.end() && block_config.find("tags")!=block_config.end()){
            _block_address = block_config["address"].get<string>();
            _block_bytes = block_config["bytes"].get<int>();
            _block_tags = block_config["tags"].get<vector<string>>();

            spdlog::info("Block Configuration : {} bytes with {} tags",_block_bytes, _block_tags.size());
        }
    }


    return true;
}

void qualDmrTask::execute(){
    auto t_now = std::chrono::high_resolution_clock::now();
    //connection
    try {
        serviceHandle& _fenetHandle = _serviceHandles[svc_fenet]; //LSIS FEnet Service
        serviceHandle& _mqttHandle = _serviceHandles[svc_mqtt];; //MQTT Service

        if(_fenetHandle.pService){
            vector<uint8_t> rcvdata = _fenetServiceAPI->read_block(_block_address, _block_bytes);
            spdlog::info("{} bytes received", rcvdata.size());

            if(rcvdata.empty())
                return;

            string msg = "Data,host=aop-super-server "; //white space required at the end of the string

            switch (_block_address.at(2))
            {
            case 'X':   //bit
                spdlog::warn("this is not allowed to read a bits of the block data");
            break;
            case 'B':   //Byte
            {
                if(_mqttHandle.pService){
                    for(int i=0;i<(int)_block_tags.size();i++){
                        msg.append(fmt::format("{}={:f}",_block_tags[i], static_cast<float>(b2f(&rcvdata[i*4]))));
                        if(i<(int)_block_tags.size()-1)
                            msg.append(",");
                    }
                    _mqttServiceAPI->publish(msg);
                }
            }
                break;
            case 'W':   //Word(2bytes)
            {
                if(_mqttHandle.pService){
                    for(int i=0;i<(int)_block_tags.size();i+=2){
                        uint16_t data = rcvdata[i]<<8 | rcvdata[i+1];
                        msg.append(fmt::format("{}={:d}",_block_tags[i], data));
                        if(i<(int)_block_tags.size()-1)
                            msg.append(",");
                    }
                    _mqttServiceAPI->publish(msg);
                }
            }
                break;
            case 'D':   //Double Word(4bytes)
            {
                if(_mqttHandle.pService){
                    for(int i=0;i<(int)_block_tags.size();i+=2){
                        uint32_t data = rcvdata[i]<<24 | rcvdata[i+1]<<16 | rcvdata[i+2]<<8 | rcvdata[i+3];
                        msg.append(fmt::format("{}={:d}",_block_tags[i], data));
                        if(i<(int)_block_tags.size()-1)
                            msg.append(",");
                    }
                    _mqttServiceAPI->publish(msg);
                }
            }
                break;
            case 'L':   //Long Word(8bytes)
            {
                spdlog::warn("this is not implemented yet.");
            }
                break;
            default:
                break;
            }
        }
    } 
    catch (jsonrpccxx::JsonRpcException &e) {
        spdlog::warn("RPC Error : {}", e.what());
    }
}

void qualDmrTask::cleanup(){

    serviceHandle& _fenetHandle = _serviceHandles[svc_fenet]; //LSIS FEnet Service
    //serviceHandle& _mongodbHandle = _serviceHandles[svc_mongo]; //LSIS MongoDB Service

    if(_fenetHandle.pfHandle){
        release_service pfRelease = (release_service)dlsym(_fenetHandle.pfHandle, "release");
        if(pfRelease)
            pfRelease();
        dlclose(_fenetHandle.pfHandle);
        _fenetHandle.pfHandle = nullptr;
    }

    // if(_mongodbHandle.pfHandle){
    //     release_service pfRelease = (release_service)dlsym(_mongodbHandle.pfHandle, "release");
    //     if(pfRelease)
    //         pfRelease();
    //     dlclose(_mongodbHandle.pfHandle);
    //     _mongodbHandle.pfHandle = nullptr;
    // }

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

float qualDmrTask::b2f(const uint8_t* bytes){
    float v = {0.0};
    memcpy(&v, bytes, sizeof(uint8_t)*4);
    return v;
}