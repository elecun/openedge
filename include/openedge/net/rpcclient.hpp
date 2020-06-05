
/**
 * @file    rpcclient.hpp
 * @brief   RPC Client Interface Class
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_NET_RPC_CLIENT_HPP_
#define _OPENEDGE_NET_RPC_CLIENT_HPP_

#include <string>

using namespace std;

namespace oe::net {
    namespace rpc {

        class rpcClientConnector {
            public:
                virtual ~rpcClientConnector() = default;
                virtual string send(const string& request) = 0;
        };

    } //namespace rpc
} //namespace oe::net


#endif