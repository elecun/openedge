
/**
 * @file    netbase.hpp
 * @brief   
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <any>

namespace oe {
    namespace net {

        class netBase {
            public:
                netBase();
                virtual ~netBase(){

                }

            protected:

            private:
                //void callback_receive();


            protected:
                int* ptrSocket = nullptr;
                struct sockaddr_in address;
        }; //class netBase

    } //namespace net
} //namespace oe