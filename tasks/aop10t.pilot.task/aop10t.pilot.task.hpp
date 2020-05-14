
/**
 * @file    aop10t.pilot.task.hpp
 * @brief   AOP 10t-scale Pilot Data Collection Task
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_AOP10T_PILOT_TASK_HPP_
#define _OPENEDGE_AOP10T_PILOT_TASK_HPP_

#include <openedge/core.hpp>
#include <string>
#include <memory>
//#include <3rdparty/jsonrpccxx/client.hpp>

using namespace oe;
using namespace std;

//class fenetConnectorServiceAPI;
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
        serviceHandle _fenetHandler; //LSIS FEnet Connector
        serviceHandle _mongoHandler; //MongoDB Connector

    private:
        // unique_ptr<fenetConnectorServiceAPI> _fenetServiceAPI;
        // unique_ptr<core::task::localServiceConnector> _fenetConnector;
        //shared_ptr<JsonRpcClient> _fenetServiceClient;
        //shared_ptr<
};

EXPORT_TASK_API

#endif

