

#include "xgb_fenet.service.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <3rdparty/spdlog/spdlog.h>


namespace oe {
    namespace PLC {
        namespace LSIS {

            XGBFEnetService::XGBFEnetService(){
                _fenet = new net::XGBFEnet();
            }

            XGBFEnetService::~XGBFEnetService(){
                delete _fenet;
            }

            bool XGBFEnetService::connect(const char* ipv4_address, int port){
                try {
                    bool rc = _fenet->connect(ipv4_address, port);
                    if(!rc){
                        return false;
                    }
                }
                catch(const std::runtime_error& e){
                    spdlog::error("Cannot connect to XGBFEnet Server");
                }
                

                //in_addr _ip_address;
                //inet_pton(AF_INET, ipv4_address, &_ip_address);

                return true;
            }

        } //namespace LSIS
    } //namesoace plc
} //namesoace oe