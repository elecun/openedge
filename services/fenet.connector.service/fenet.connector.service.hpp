
/**
 * @file    fenet.connector.service.hpp
 * @brief   LSIS PLC Service
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_FENET_CONNECTOR_HPP_
#define _OPENEDGE_SERVICE_FENET_CONNECTOR_HPP_

#include <openedge/core/export.hpp>
#include <openedge/core/typespec.hpp>
#include <openedge/core/service.hpp>
#include <openedge/core/device.hpp>
#include <openedge/core/bus.hpp>
#include <stdint.h>

using namespace oe::core;

class EXPORTED fenetConnectorService : public iService {
    public:
        fenetConnectorService();
        virtual ~fenetConnectorService();
        
        //common service interface
        bool initService() override;

        //common PLC interface
        bool readBit(bus::iDeviceBus* bus, iProtocolRaw* protocol, const char* address);
        uint8_t readByte(bus::iDeviceBus* bus, iProtocolRaw* protocol, const char* address);
        uint16_t readWord(bus::iDeviceBus* bus, iProtocolRaw* protocol, const char* address);
        uint32_t readDword(bus::iDeviceBus* bus, iProtocolRaw* protocol, const char* address);
        uint64_t readLword(bus::iDeviceBus* bus, iProtocolRaw* protocol, const char* address);

    private:
        uint16_t _invokeId { 0x0000 };

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