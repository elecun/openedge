

#ifndef _OPENEDGE_SERVICE_LSIS_FENET_CONNECTOR_API_HPP_
#define _OPENEDGE_SERVICE_LSIS_FENET_CONNECTOR_API_HPP_

#include <3rdparty/jsonrpccxx/client.hpp>

using namespace jsonrpccxx;

class fenetConnectorServiceAPI {
    public:
        explicit fenetConnectorServiceAPI(JsonRpcClient& client):_client(client){}
        bool test(const int& value){ return _client.CallMethod<bool>(1, "test", {value}); }
    private:
        JsonRpcClient& _client;
};



class fenetServiceAPI {
public:
  explicit fenetServiceAPI(JsonRpcClient &client) : client(client) {}
  bool test(const int& value) { return client.CallMethod<bool>(1, "test", {value}); }
private:
  JsonRpcClient& client;
};

#endif