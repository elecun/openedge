
/**
 * @file    aop10t.pilot.task.hpp
 * @brief   AOP 10t-scale Pilot Data Collection Task
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_AOP10T_PILOT_TASK_HPP_
#define _OPENEDGE_AOP10T_PILOT_TASK_HPP_

#include <string>
#include <map>
#include <memory>
#include <openedge/core.hpp>
#include <fstream>

using namespace oe;
using namespace std;

namespace jsonrpccxx{ class JsonRpcClient; }

class fenetServiceAPI;
class mongoServiceAPI;
class mqttServiceAPI;
class aop10tPilotTask : public oe::core::task::runnable {
    public:
        aop10tPilotTask() = default;
        virtual ~aop10tPilotTask() = default;

        //common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;

    private:
        bool _load_service(serviceHandle& service /*out*/);

    private:
        map<string, serviceHandle> _serviceHandles;  //service handle map

        unique_ptr<core::task::localServiceConnector> _fenetConnector;  //LSIS FEnet Service Connector
        unique_ptr<core::task::localServiceConnector> _mongoConnector;  //MongoDB Service Connector

        shared_ptr<jsonrpccxx::JsonRpcClient> _fenetAccessor;    //for FEnet service access
        unique_ptr<fenetServiceAPI> _fenetServiceAPI; //fenet service API

        shared_ptr<jsonrpccxx::JsonRpcClient> _mqttPublisher;    //for FEnet service access
        unique_ptr<mqttServiceAPI> _mqttServiceAPI; //fenet service API

        shared_ptr<jsonrpccxx::JsonRpcClient> _mongoAccessor;   //for MongoDB Service access
        unique_ptr<mongoServiceAPI> _mongoServiceAPI;   //mongodb service API

        
        std::ofstream _logPerf;
};

EXPORT_TASK_API

#endif


