

#ifndef _OPENEDGE_SERVICE_MODBUS_RTU_API_HPP_
#define _OPENEDGE_SERVICE_MODBUS_RTU_API_HPP_

#include <3rdparty/jsonrpccxx/client.hpp>

using namespace jsonrpccxx;

namespace oe::service {
    class modbusRtuServiceAPI {
        public:
            explicit modbusRtuServiceAPI(JsonRpcClient& client):_client(client){}
            
            unsigned char read(const unsigned char address) { return _client.CallMethod<unsigned char>(1, "read", {address}); }
        private:
            JsonRpcClient& _client;
    };
}

#endif