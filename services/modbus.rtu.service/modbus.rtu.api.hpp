

#ifndef _OPENEDGE_SERVICE_MODBUS_RTU_API_HPP_
#define _OPENEDGE_SERVICE_MODBUS_RTU_API_HPP_

#include <3rdparty/jsonrpccxx/client.hpp>
#include <vector>

using namespace jsonrpccxx;

namespace oe::service {
    class modbusRtuServiceAPI {
        public:
            explicit modbusRtuServiceAPI(JsonRpcClient& client):_client(client){}
            
            vector<uint16_t> read_holding_registers(const uint16_t address, int size) { return _client.CallMethod<vector<uint16_t>>(1, "read_holding_registers", {address, size}); }
            bool write_holding_register(const uint16_t address, uint16_t data) { return _client.CallMethod<bool>(2, "write_holding_register", { address, data }); }

        private:
            JsonRpcClient& _client;
    };
}

#endif