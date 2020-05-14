

#ifndef _OPENEDGE_SERVICE_LSIS_FENET_CONNECTOR_API_HPP_
#define _OPENEDGE_SERVICE_LSIS_FENET_CONNECTOR_API_HPP_


#include <3rdparty/jsonrpccxx/client.hpp>

using namespace jsonrpccxx;

class fenetServiceAPI {
    public:
        explicit fenetServiceAPI(JsonRpcClient& client):_client(client){}
        bool test(const int& value){ return _client.CallMethod<bool>(1, "test", {value}); }
        bool write(const std::string& data){ return _client.CallMethod<bool>(2, "write", {data}); }

    private:
        JsonRpcClient& _client;
};

#endif