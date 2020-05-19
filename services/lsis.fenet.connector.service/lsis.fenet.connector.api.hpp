

#ifndef _OPENEDGE_SERVICE_LSIS_FENET_CONNECTOR_API_HPP_
#define _OPENEDGE_SERVICE_LSIS_FENET_CONNECTOR_API_HPP_


#include <3rdparty/jsonrpccxx/client.hpp>

using namespace jsonrpccxx;

class fenetServiceAPI {
    public:
        explicit fenetServiceAPI(JsonRpcClient& client):_client(client){}
        string read(const std::string& address){ return _client.CallMethod<string>(2, "read", {address}); } //ex. '%MW0'
        string read_n(const std::string& address, int count){ return _client.CallMethod<string>(3, "read_n", {address, count}); }

    private:
        JsonRpcClient& _client;
};

#endif