
/**
 * @file    lsis.fenet.connector.service.hpp
 * @brief   FEnet Connector Service (depends on XGT dedicated protocol over TCP/IP Communication)
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_LSIS_FENET_CONNECTOR_HPP_
#define _OPENEDGE_SERVICE_LSIS_FENET_CONNECTOR_HPP_

#include <openedge/core/service.hpp>
#include <3rdparty/sockpp/tcp_connector.h>
#include <string>
#include <memory>
#include <vector>

using namespace oe;
using namespace std;


namespace oe::bus::protocol { class XGTDedicated; }
class fenetConnectorService : public core::iService {
    public:
        fenetConnectorService();
        virtual ~fenetConnectorService();
        
        //common iservice interface
        bool initService(const char* config = nullptr) override;
        bool closeService() override;

        //services APIs
        bool test(const int& value);
        string read(const std::string& address); //ex. %MW0
        string read_n(const std::string& address, int count);

    private:
        sockpp::tcp_connector _fenetConnector;
        unique_ptr<oe::bus::protocol::XGTDedicated> _protocol;

}; //class

EXPORT_SERVICE_API

#endif