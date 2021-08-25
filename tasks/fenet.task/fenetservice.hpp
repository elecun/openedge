/**
 * @file    fenetService.hpp
 * @brief   LSIS FENet Service Class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_FENET_
#define _OPENEDGE_SERVICE_FENET_

#include <openedge/core/service.hpp>
#include <3rdparty/sockpp/tcp_connector.h>
#include "protocol.hpp"
#include <vector>

using namespace std;


namespace oe::support {

    class FENetService : public oe::core::service {
        public:
            FENetService(const char* address, int port):_address(address), _port(port){
                //connect
            }
            virtual ~FENetService() = default;

            bool init() {
                return true;
            }
            bool request() {
                return true;
            }

            bool valid() {
                return true;
            }

            vector<unsigned char> block_read(){
                return vector<unsigned char>();
            }

        private:
            string _address { "127.0.0.1"};
            int _port { 0 };

        private:
            sockpp::tcp_connector _connector;
            unique_ptr<oe::support::protocol::XGTDedicated> _protocol;


    }; /* FENetService Class */
} /* namespace */

#endif