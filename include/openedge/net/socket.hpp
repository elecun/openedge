
/**
 * @file    socket.hpp
 * @brief   socket base class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_NET_SOCKET_HPP_
#define _OPENEDGE_NET_SOCKET_HPP_

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

namespace oe::net {

    class socket {
        public:

            #define MAX_RCVBUF_SIZE 2048

            socket():_socket(-1) {
                memset(&_ipv4_address, 0, sizeof(_ipv4_address));
                _socket = ::socket(AF_INET , SOCK_STREAM , 0);
            }
            virtual ~socket(){
                if(is_valid())
                    ::close(_socket);
            }

            bool connect(const char* host, const int port){
                if(!is_valid())
                    return false;

                _ipv4_address.sin_family = AF_INET;
                _ipv4_address.sin_port = htons(port);

                inet_pton(AF_INET, host, &_ipv4_address.sin_addr);
                int status = ::connect(_socket, (sockaddr*)&_ipv4_address, sizeof (_ipv4_address));
                if(!status)
                    return true;
                else
                    return false;
            }

            bool send(unsigned char* byte, int len){
                int nsent = ::send(_socket, byte, len, MSG_NOSIGNAL);
                if(nsent!=-1)
                    return true;
                return false;
            }

            int receive(unsigned char* byte) const {
                char rcv_buffer[MAX_RCVBUF_SIZE+1] = {0,};
                memset(rcv_buffer, 0, sizeof(rcv_buffer));

                int nbytes = ::recv(_socket,rcv_buffer, sizeof(rcv_buffer),0);
                memcpy(byte, &rcv_buffer, sizeof(rcv_buffer));

                return nbytes;
            }

            bool is_valid() const { return _socket != -1; }

        private:
            int _socket;
            sockaddr_in _ipv4_address;
            
    }; //class

} //namespace 

#endif