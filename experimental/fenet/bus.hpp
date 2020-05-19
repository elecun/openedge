

#ifndef _BUS_HPP_
#define _BUS_HPP_

#include "protocol.hpp"
#include "socket.hpp"

namespace oe::bus {

    class ethernet {
        public:
            
            ethernet() {

            }
            ethernet(net::protocol::XGTDedicated* protocol){

            }
            ethernet(net::protocol::XGTDedicated* protocol, const char* host_ip4v = nullptr, const int port = 0){
                 _tcp_socket = new net::socket()
            }

            ~ethernet() {
                delete _tcp_socket;
            }

            void configHost(const char* ipv4, const int port){

            }

        private:
            net::socket* _tcp_socket = nullptr;
            

    }; //class
}


#endif