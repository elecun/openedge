
/**
 * @file    xgt.protocol.service
 * @brief   LSIS XGT Dedicated Protocol Service Class
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_XGT_PROTOCOL_HPP_
#define _OPENEDGE_SERVICE_XGT_PROTOCOL_HPP_

#include <openedge/core/export.hpp>
#include <openedge/core/service.hpp>
#include <openedge/core/protocol.hpp>
#include <stdint.h>

using namespace oe;

class EXPORTED xgtProtocolService : public core::iService, public core::iProtocolRaw {
    public:
        xgtProtocolService();
        virtual ~xgtProtocolService();
        
        //common service interface
        bool initService() override;

        //common raw protocol interface        
        vector<uint8_t> getHeader() override;


}; //class


#ifdef  __cplusplus
extern "C" {
#endif

xgtProtocolService* pService = nullptr;

EXPORTED oe::core::iService* createService(void) { pService = new xgtProtocolService(); return pService; } //not static
EXPORTED void releaseService(void) { if(pService) { delete pService; pService=nullptr; }}
EXPORTED bool initService(void) { if(pService) return pService->initService(); return false; }

#ifdef  __cplusplus
}
#endif

#endif