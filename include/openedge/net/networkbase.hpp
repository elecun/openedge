
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
#include <arpa/inet.h>

using namespace std;

namespace oe::net {

    class networkBase {
        public:
            networkBase();
            virtual ~networkBase(){
                delete socket;
            }

            bool getUseSynchronousNet() const;
            void setUseSynchronousNet(bool value);

            virtual const char* getIpAddress() const {
                return (const char*)_ipv4_address;
            }
            virtual void setIpAddress(const char* ipaddr){
                memcpy(_ipv4_address, ipaddr, sizeof(_ipv4_address));
            }
            virtual void setIpAddress(sockaddr_in ipaddr){
                inet_ntop(AF_INET, &ipaddr, _ipv4_address, INET_ADDRSTRLEN);
            }

            virtual int getPort() const { return _port; }
            virtual int setPort(int port) { _port = port; }

            const char* getConnectionId() const { return _connectionId.c_str(); }
            void setConnectionId(const char* id){ _connectionId = id; }

            bool connect() {
                socket==nullptr?nullptr:
            }
            


        protected:
            void threadCheckTimeout(std::any obj);
            std::vector<uint8_t>* receive(int* socket, int length);

            int* createSocketandConnect(const char* ipv4_addr, int port);


        private:
            void receiveCallback();

        protected:
            int* socket = nullptr;

        private:
            bool _useSynchronousNet = false;
            char _ipv4_address[INET_ADDRSTRLEN];
            //struct sockaddr_in _ipv4_address;
            int _port;
            string _connectionId = "";


            
            
    }; //class netBase

} //namespace net