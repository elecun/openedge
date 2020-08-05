

#ifndef _OPENEDGE_SERVICE_MQTT_PUBLISHER_API_HPP_
#define _OPENEDGE_SERVICE_MQTT_PUBLISHER_API_HPP_


#include <3rdparty/jsonrpccxx/client.hpp>
#include <string>

using namespace jsonrpccxx;

namespace oe::service {
    class avahiMdnsServiceAPI {
        public:
            explicit avahiMdnsServiceAPI(JsonRpcClient& client):_client(client){}
            
            bool publish(const std::string& data) { return _client.CallMethod<bool>(1, "publish", {data}); }
            bool drop() { return _client.CallMethod<bool>(2, "drop"); }
            
        private:
            JsonRpcClient& _client;
    };
}

#endif