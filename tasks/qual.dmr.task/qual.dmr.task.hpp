
/**
 * @file    qual.dmr.task.hpp
 * @brief   Task for DMR Qualification Test
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_QUALIFICATION_DMR_TASK_HPP_
#define _OPENEDGE_QUALIFICATION_DMR_TASK_HPP_

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <openedge/core.hpp>

namespace jsonrpccxx{ class JsonRpcClient; }
namespace oe::net::rpc { class client; }

namespace oe::service {
    class modbusRtuServiceAPI;  //Modbus RTU Service API
}

using namespace oe;
using namespace std;
using namespace oe::service;

class qualDmrTask : public oe::core::task::runnable {
    public:
        qualDmrTask() = default;
        virtual ~qualDmrTask() = default;

        //common task interfaces
        bool configure() override;
        void execute() override;
        void cleanup() override;

    private:
        bool _load_service(serviceHandle& service /*out*/);

    private:
        map<string, serviceHandle> _serviceHandles;  //service handle map

        unique_ptr<core::task::localServiceConnector> _modbusRtuConnector;  //modbus RTU Service Connector
        shared_ptr<jsonrpccxx::JsonRpcClient> _modbusRtuAccessor;    //for modbus RTU service access
        unique_ptr<modbusRtuServiceAPI> _modbusRtuServiceAPI; //modbus RTU service API

        unsigned char _read_address = 0x00;
        unsigned char _read_function = 0x03;
        
};

EXPORT_TASK_API

#endif


