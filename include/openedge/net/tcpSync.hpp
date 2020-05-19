
/**
 * @file    tcpSync.hpp
 * @brief   synchronous TCP socket base class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_NET_TCP_SYNC_HPP_
#define _OPENEDGE_NET_TCP_SYNC_HPP_

#include "sockbase.hpp"

namespace oe::net::tcp {

    class sync : public sockbase {
        public:
            sync(unsigned long long timeout = 1 /*waiting 1 nanosec*/){ }
            virtual ~sync() = default;

            virtual bool connet(const char* ipv4_addr, int port) {

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