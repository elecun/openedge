

#ifndef _OPENEDGE_SERVICE_MONGODB_CONNECTOR_API_HPP_
#define _OPENEDGE_SERVICE_MONGODB_CONNECTOR_API_HPP_


#include <3rdparty/jsonrpccxx/client.hpp>

using namespace jsonrpccxx;

class mongoServiceAPI {
    public:
        explicit mongoServiceAPI(JsonRpcClient& client):_client(client){}
        bool test(const int& value){ return _client.CallMethod<bool>(1, "test", {value}); }
    private:
        JsonRpcClient& _client;
};

#endif