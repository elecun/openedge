
/**
 * @file    bus.tcp.service.hpp
 * @brief   Ethernet TCP Bus Service Clsss
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_BUS_TCP_HPP_
#define _OPENEDGE_SERVICE_BUS_TCP_HPP_

#include <openedge/core/export.hpp>
#include <openedge/core/service.hpp>
#include <openedge/core/bus.hpp>
#include <stdint.h>
#include <openedge/net/socket.hpp>

using namespace oe;

class EXPORTED busTcpService : public core::iService, public core::bus::iDeviceBusTCP {
    public:
        busTcpService();
        virtual ~busTcpService();

        bool open() override;
        void close() override;
        int read(uint8_t* data, int len) override;
	    int write(const uint8_t* data, int len) override;
        
        //common service interface
        bool initService() override;

        bool connect(const char* ipv4_address, const int port);
        void disconnect();

    private:
        net::socket* _asyncSocket = nullptr;


}; //class


#ifdef  __cplusplus
extern "C" {
#endif

busTcpService* pService = nullptr;

EXPORTED oe::core::iService* createService(void) { pService = new busTcpService(); return pService; } //not static
EXPORTED void releaseService(void) { if(pService) { delete pService; pService=nullptr; }}
EXPORTED bool initService(void) { if(pService) return pService->initService(); return false; }
//EXPORTED uint8_t readByte(const char* address) { return pService->readByte(address); } //hide

#ifdef  __cplusplus
}
#endif

#endif