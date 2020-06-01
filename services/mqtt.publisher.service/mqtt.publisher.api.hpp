

#ifndef _OPENEDGE_SERVICE_MQTT_PUBLISHER_API_HPP_
#define _OPENEDGE_SERVICE_MQTT_PUBLISHER_API_HPP_


#include <3rdparty/jsonrpccxx/client.hpp>

using namespace jsonrpccxx;

class mqttServiceAPI {
    public:
        explicit mqttServiceAPI(JsonRpcClient& client):_client(client){}
        
        bool publish(const string& data) { return _client.CallMethod<bool>(1, "publish", {data}); }
    private:
        JsonRpcClient& _client;
};

#endif