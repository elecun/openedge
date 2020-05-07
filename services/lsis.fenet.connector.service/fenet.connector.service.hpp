
/**
 * @file    fenet.connector.service.hpp
 * @brief   FEnet Connector Service (depends on XGT dedicated protocol over TCP/IP Communication)
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_FENET_CONNECTOR_HPP_
#define _OPENEDGE_SERVICE_FENET_CONNECTOR_HPP_

#include <openedge/core/export.hpp>
#include <openedge/core/typespec.hpp>
#include <openedge/core/service.hpp>
#include <openedge/core/device.hpp>
#include <openedge/core/bus.hpp>
#include <openedge/net/tcpAsync.hpp>
#include <stdint.h>
#include <array>

using namespace oe;

class EXPORTED fenetConnectorService : public core::iService, net::tcp::async {
    public:
        fenetConnectorService();
        virtual ~fenetConnectorService();
        
        //common iservice interface
        bool initService() override;

        //common public net::tcpbase interface
        bool connet(const char* ipv4_addr, int port) override;

        //support interface for FEnet
        bool connect(const char* ipv4, int port);
        void request( const char* addr_start, /* start address to access */
                    uint16_t count = 0      /*data count to read. if 0, it is individual request*/
                );

    private:
        void parse(const char* address = nullptr); /* parse address */
        
        //common private net::tcp interface
        void on_received() override;

    private:
        uint16_t _invokeId { 0x0000 };
        unique_ptr<core::bus::iDeviceBus> _bus;// = unique_ptr<bus::iDeviceBus>(nullptr); /* default is null */

}; //class


#ifdef  __cplusplus
extern "C" {
#endif

fenetConnectorService* pService = nullptr;

EXPORTED oe::core::iService* createService(void) { pService = new fenetConnectorService(); return pService; } //not static
EXPORTED void releaseService(void) { if(pService) { delete pService; pService=nullptr; }}
EXPORTED bool initService(void) { if(pService) return pService->initService(); return false; }
//EXPORTED uint8_t readByte(const char* address) { return pService->readByte(address); } //hide

#ifdef  __cplusplus
}
#endif

#endif