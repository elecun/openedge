
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

using namespace oe;
using namespace std;

namespace jsonrpccxx{ class JsonRpcClient; }

class fenetServiceAPI;
class mongoServiceAPI;
class aop10tPilotTask : public oe::core::task::runnable {
    public:
        aop10tPilotTask() = default;
        virtual ~aop10tPilotTask() = default;

        //common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;

    private:

        //load specific service
        bool _load_fenet_service();
        bool _load_mongo_service();

    private:
        serviceHandle _fenetHandle; //LSIS FEnet Service component handle (componet file handler)
        serviceHandle _mongoHandle; //MongoDB Service component handle (component file handler)

        unique_ptr<core::task::localServiceConnector> _fenetConnector;  //LSIS FEnet Service Connector
        unique_ptr<core::task::localServiceConnector> _mongoConnector;  //MongoDB Service Connector

        shared_ptr<jsonrpccxx::JsonRpcClient> _fenetAccessor;    //for FEnet service access
        unique_ptr<fenetServiceAPI> _fenetServiceAPI; //fenet service API

        shared_ptr<jsonrpccxx::JsonRpcClient> _mongoAccessor;   //for MongoDB Service access
        unique_ptr<mongoServiceAPI> _mongoServiceAPI;   //mongodb service API
};

EXPORT_TASK_API

#endif


