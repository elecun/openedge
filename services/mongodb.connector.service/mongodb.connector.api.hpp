

#ifndef _OPENEDGE_SERVICE_MONGODB_CONNECTOR_API_HPP_
#define _OPENEDGE_SERVICE_MONGODB_CONNECTOR_API_HPP_


#include <3rdparty/jsonrpccxx/client.hpp>

using namespace jsonrpccxx;

namespace oe::service {
    class mongoServiceAPI {
        public:
            explicit mongoServiceAPI(JsonRpcClient& client):_client(client){}
            
            bool insert(const string& document) { return _client.CallMethod<bool>(3, "insert", {document}); }
        private:
            JsonRpcClient& _client;
    };
}

#endif