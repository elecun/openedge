

#ifndef _OPENEDGE_SERVICE_LSIS_FENET_CONNECTOR_API_HPP_
#define _OPENEDGE_SERVICE_LSIS_FENET_CONNECTOR_API_HPP_


#include <3rdparty/jsonrpccxx/client.hpp>
#include <vector>
#include <stdint.h>
#include <string>

using namespace jsonrpccxx;
using namespace std;

namespace oe::service {
    class fenetServiceAPI {
        public:
            explicit fenetServiceAPI(JsonRpcClient& client):_client(client){}
            string read(const std::string& address){ return _client.CallMethod<string>(2, "read", {address}); } //ex. '%MW0'
            vector<uint8_t> read_block(const std::string& address, int size) { return _client.CallMethod<vector<uint8_t>>(4, "read_block", {address, size});}

        private:
            JsonRpcClient& _client;
    };
}

#endif