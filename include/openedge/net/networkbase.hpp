
/**
 * @file    networkbase.hpp
 * @brief   network base class with socket
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <any>
#include <openedge/util/uuid.hpp>
#include <vector>

using namespace std;

namespace oe::net {

    class networkBase {
        public:
            networkBase();
            virtual ~networkBase(){
                delete socket;
            }

            oe::util::uuid_t getToken() const;
            void setToken(oe::util::uuid_t value);

            bool getUseSynchronousNet() const;
            void setUseSynchronousNet(bool value);

        protected:
            void threadCheckTimeout(std::any obj);
            std::vector<uint8_t>* receive(int* socket, int length);

            int* createSocketandConnect(const char* ipv4_addr, int port);


        private:
            void receiveCallback();


        protected:
            int* socket = nullptr;
            struct sockaddr_in address;

        private:
            oe::util::uuid_t _token;
            bool _useSynchronousNet = false;
            
    }; //class netBase

} //namespace net