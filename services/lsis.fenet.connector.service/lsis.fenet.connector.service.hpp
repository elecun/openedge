
/**
 * @file    lsis.fenet.connector.service.hpp
 * @brief   FEnet Connector Service (depends on XGT dedicated protocol over TCP/IP Communication)
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_LSIS_FENET_CONNECTOR_HPP_
#define _OPENEDGE_SERVICE_LSIS_FENET_CONNECTOR_HPP_

#include <openedge/core/service.hpp>
#include <3rdparty/sockpp/tcp_connector.h>

using namespace oe;

class fenetConnectorService : public core::iService {
    public:
        fenetConnectorService();
        virtual ~fenetConnectorService();
        
        //common iservice interface
        bool initService() override;
        bool closeService() override;

    private:
        void parse(const char* address = nullptr); /* parse address */

    private:
        uint16_t _invokeId { 0x0000 };

}; //class

EXPORT_SERVICE_API

#endif