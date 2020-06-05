
/**
 * @file    jsonrpc.hpp
 * @brief   JSON RPC Interface Class
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_NET_JSONRPC_HPP_
#define _OPENEDGE_NET_JSONRPC_HPP_

#include <string>
#include <3rdparty/jsonrpccxx/server.hpp>

using namespace std;

namespace oe::net {
    namespace rpc {

        /**
         * @brief   RPC Client Interface
         */
        class rpcClientConnector {
            public:
                virtual ~rpcClientConnector() = default;
                virtual string send(const string& request) = 0;
        };

        /**
         * @brief   Local Service Connector with RPC
         */
        class localServiceConnector : public rpcClientConnector {
            public:
                explicit localServiceConnector(jsonrpccxx::JsonRpcServer& server):server(server){}
                string send(const string& request) override { return server.HandleRequest(request); }
            private:
                jsonrpccxx::JsonRpcServer& server;
        };

    } //namespace rpc
} //namespace oe::net


#endif