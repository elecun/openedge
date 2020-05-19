
/**
 * @file    sockbase.hpp
 * @brief   Socket Base
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_NET_SOCKBASE_HPP_
#define _OPENEDGE_NET_SOCKBASE_HPP_

#if defined(__linux__)
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif

#include <functional>
#include <3rdparty/spdlog/spdlog.h>

using namespace std;

namespace oe::net {

    class sockbase {
        public:
            typedef enum socktype {
                tcp = SOCK_STREAM,
                udp = SOCK_DGRAM
            };

            explicit sockbase(socktype type = socktype::tcp, int socket = -1){
                if((sock = ::socket(AF_INET, type,  0))<0)
                    spdlog::error("Could not create socket");
            }

            virtual void close() {
                ::close(sock);
            }

            virtual bool connect(const char* host, int port){
                ipv4_addr.sin_family = AF_INET;
                ipv4_addr.sin_port = htons(port);

                inet_pton(AF_INET, host, &ipv4_addr.sin_addr);
                int status = ::connect(sock, (sockaddr*)&ipv4_addr, sizeof (ipv4_addr));

                return (!status)?true:false;
            }

        protected:
            int sock = 0;
            sockaddr_in ipv4_addr;


    }; //class
} //namespace

#endif