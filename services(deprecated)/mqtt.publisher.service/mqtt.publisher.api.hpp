

#ifndef _OPENEDGE_SERVICE_MQTT_PUBLISHER_API_HPP_
#define _OPENEDGE_SERVICE_MQTT_PUBLISHER_API_HPP_


#include <3rdparty/jsonrpccxx/client.hpp>
#include <string>

using namespace jsonrpccxx;

namespace oe::service {
    class mqttServiceAPI {
        public:
            explicit mqttServiceAPI(JsonRpcClient& client):_client(client){}
            
            bool publish(const std::string& data) { return _client.CallMethod<bool>(1, "publish", {data}); }
            
        private:
            JsonRpcClient& _client;
    };
}

#endif