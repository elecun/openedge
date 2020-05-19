
/**
 * @file    netdevbase.hpp
 * @brief   
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

namespace oe {
    namespace net {

        class netDevBase {
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