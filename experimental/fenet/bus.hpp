

#ifndef _BUS_HPP_
#define _BUS_HPP_

#include "protocol.hpp"

namespace oe::bus {

    class ethernet {
        public:
            
            ethernet() {

            }
            ethernet(net::protocol::XGTDedicated* protocol){

            }
            ethernet(net::protocol::XGTDedicated* protocol, const char* host_ip4v = nullptr, const int port = 0){

            }

            ~ethernet() {

            }

            void configHost(const char* ipv4, const int port){

            }

        protected:
            

    }; //class
}


#endif