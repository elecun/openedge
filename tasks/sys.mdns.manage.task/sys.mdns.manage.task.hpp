
/**
 * @file    sys.mdns.manage.task.hpp
 * @brief   service publish/browse management task with mDNS/DNS-SD
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SYS_MDNS_MANAGE_TASK_HPP_
#define _OPENEDGE_SYS_MDNS_MANAGE_TASK_HPP_

#include <openedge/core.hpp>

namespace jsonrpccxx{ class JsonRpcClient; }
namespace oe::net::rpc { class client; }

namespace oe::service {
    // class fenetServiceAPI;      //LSIS PLC Fenet Service API
    // class mongoServiceAPI;      //MongoDB Service API
    // class mqttServiceAPI;       //MQTT Service API
}

using namespace oe;
using namespace std;
using namespace oe::service;

class sysMdnsManageTask : public oe::core::task::runnable {
    public:
        sysMdnsManageTask() = default;
        virtual ~sysMdnsManageTask() = default;

        //common task interfaces
        bool configure() override;
        void execute() override;
        void cleanup() override;
        
};

EXPORT_TASK_API

#endif