
/**
 * @file    tcpAsync.hpp
 * @brief   Asynchronous TCP socket base class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_NET_TCP_ASYNC_HPP_
#define _OPENEDGE_NET_TCP_ASYNC_HPP_

#include "sockbase.hpp"

namespace oe::net::tcp {

    class async : public sockbase {
        public:
            async(bool keepAlive = false){ }
            virtual ~async() = default;

            virtual bool connect(const char* ipv4_addr, int port) {

            }

            virtual void close() {

            }
            
            virtual int write(byte* data, unsigned int len){

            }

        protected:

            virtual void on_received() {

            }

            virtual void on_closed() {

            }


    }; //class

} //namespace

#endif