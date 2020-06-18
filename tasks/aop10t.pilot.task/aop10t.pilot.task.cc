

#include "aop10t.pilot.task.hpp"
#include <3rdparty/spdlog/spdlog.h>
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

    if(_serviceHandles.find(svc_mongo)!=_serviceHandles.end()){
        _mongoConnector = make_unique<core::task::localServiceConnector>(_serviceHandles[svc_mongo].pService->getServicePort());
        _mongoAccessor = make_shared<jsonrpccxx::JsonRpcClient>(*_mongoConnector.get(), jsonrpccxx::version::v2);
        _mongoServiceAPI = make_unique<mongoServiceAPI>(*_mongoAccessor.get());
    }

    if(_serviceHandles.find(svc_mqtt)!=_serviceHandles.end()){
        _mqttConnector = make_unique<core::task::localServiceConnector>(_serviceHandles[svc_mqtt].pService->getServicePort());
        _mqttPublisher = make_shared<jsonrpccxx::JsonRpcClient>(*_mqttConnector.get(), jsonrpccxx::version::v2);
        _mqttServiceAPI = make_unique<mqttServiceAPI>(*_mqttPublisher.get());
    }

    //configure a task activity
    if(_serviceHandles[svc_fenet].pService){
        json activity = json::parse(getProfile()->get("activity"));
        json block_config = activity["read_block"];
        if(block_config.find("address")!=block_config.end() && block_config.find("size")!=block_config.end() && block_config.find("tags")!=block_config.end()){
            _block_address = block_config["address"].get<string>();
            _block_size = block_config["size"].get<int>();
            _block_tags = block_config["tags"].get<vector<string>>();
            if(_block_tags.size()!=_block_size){
                spdlog::error("Block size is not equal to number of tags. please check task configuration file.");
                return false;
            }
        }
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

        if(_fenetHandle.pService){
            vector<uint8_t> rcvdata = _fenetServiceAPI->read_block(_block_address, _block_size);
            spdlog::info("Received Size : {}", rcvdata.size());

            if(rcvdata.empty())
                return;

            string msg = "Data,host=aop-super-server "; //white space required at the end of the string

            switch (_block_address.at(2))
            {
            case 'X':   //bit
            break;
            case 'B':   //Byte
            {
                if(_mqttHandle.pService){
                    for(int i=0;i<_block_size;i++){
                        msg.append(fmt::format("{}={:d}",_block_tags[i], static_cast<int>(rcvdata[i])));
                        if(i<_block_size-1)
                            msg.append(",");
                    }
                    _mqttServiceAPI->publish(msg);
                }
            }
                break;
            case 'W':   //Word(2bytes)
            {
                if(_mqttHandle.pService){
                    for(int i=0;i<_block_size;i+=2){
                        uint16_t data = rcvdata[i]<<8 | rcvdata[i+1];
                        msg.append(fmt::format("{}={:d}",_block_tags[i], data));
                        if(i<_block_size-1)
                            msg.append(",");
                    }
                    _mqttServiceAPI->publish(msg);
                }
            }
                break;
            case 'D':   //Double Word(4bytes)
            {
                if(_mqttHandle.pService){
                    for(int i=0;i<_block_size;i+=4){
                        uint32_t data = rcvdata[i]<<24 | rcvdata[i+1]<<16 | rcvdata[i+2]<<8 | rcvdata[i+3];
                        msg.append(fmt::format("{}={:d}",_block_tags[i], data));
                        if(i<_block_size-1)
                            msg.append(",");
                    }
                    _mqttServiceAPI->publish(msg);
                }
            }
                break;
            case 'L':   //Long Word(8bytes)
            {

            }
                break;
            default:
                break;
            }
        }

        // if(_fenetHandle.pService){
    

        //     if(block.find("address")!=block.end() && block.find("size")!=block.end()){
                

        //         vector<uint8_t> rawdata = _fenetServiceAPI->read_block(address, size);

        //         spdlog::info("Received raw data size {}", rawdata.size());
        //         switch(address.at(2)){
        //             case 'B':
        //             case 'W':
        //             {
        //                 if(_mqttHandle.pService){
        //                     string msg = "Data,host=aop-super-server ";
        //                     int start_addr = std::stoi(address.substr(3));
        //                     for(int i=0;i<size;i++){ //block size
        //                         msg.append(fmt::format("{}={:d}",namelist[i], static_cast<int>(rawdata[i])));
        //                         if(i<size-1)
        //                             msg.append(",");
        //                     }
        //                     _mqttServiceAPI->publish(msg);
        //                 }
        //             }
        //             break;
        //             case 'D': //double word
        //                 {
        //                     if(_mqttHandle.pService){
        //                     }
        //                 }
        //             break;
        //         }                
        //     }
        // }
    } 
    catch (jsonrpccxx::JsonRpcException &e) {
        spdlog::warn("RPC Error : {}", e.what());
    }
}

void aop10tPilotTask::cleanup(){

    serviceHandle& _fenetHandle = _serviceHandles[svc_fenet]; //LSIS FEnet Service
    serviceHandle& _mongodbHandle = _serviceHandles[svc_mongo]; //LSIS MongoDB Service

    if(_fenetHandle.pfHandle){
        release_service pfRelease = (release_service)dlsym(_fenetHandle.pfHandle, "release");
        if(pfRelease)
            pfRelease();
        dlclose(_fenetHandle.pfHandle);
        _fenetHandle.pfHandle = nullptr;
    }

    if(_mongodbHandle.pfHandle){
        release_service pfRelease = (release_service)dlsym(_mongodbHandle.pfHandle, "release");
        if(pfRelease)
            pfRelease();
        dlclose(_mongodbHandle.pfHandle);
        _mongodbHandle.pfHandle = nullptr;
    }

    spdlog::info("Cleanup the aop10tPilotTask");
}

bool aop10tPilotTask::_load_service(serviceHandle& service){
    
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