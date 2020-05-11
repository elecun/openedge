
/**
 * @file    lsis.fenet.connector.service.hpp
 * @brief   FEnet Connector Service (depends on XGT dedicated protocol over TCP/IP Communication)
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_LSIS_FENET_CONNECTOR_HPP_
#define _OPENEDGE_SERVICE_LSIS_FENET_CONNECTOR_HPP_

#include <openedge/core/export.hpp>
#include <openedge/core/service.hpp>
#include <3rdparty/sockpp/tcp_connector.h>
#include <openedge/core/bus.hpp>

using namespace oe;

class EXPORTED fenetConnectorService : public core::iService {
    public:
        fenetConnectorService();
        virtual ~fenetConnectorService();
        
        //common iservice interface
        bool initService() override;

        //for socket interface
        // bool connect(const char* ipv4_address, const int port) override;
        // void disconnect() override;
        // void setRcvTimeout(unsigned int sec);
        // void setReadCallback(std::function<int(vector<byte>)>& func);

        //support interface for FEnet
        // void request( const char* addr_start, /* start address to access */
        //             uint16_t count = 0      /*data count to read. if 0, it is individual request*/
        //         );


        // typedef std::function<void(uint8_t*, int)> readCallbackFunc;

    private:
        void parse(const char* address = nullptr); /* parse address */

        //for tcp socket
        sockpp::socket_initializer* _sockInit { nullptr };
        sockpp::tcp_connector _tcp;

    private:
        uint16_t _invokeId { 0x0000 };
        //unique_ptr<core::bus::iDeviceBus> _bus;// = unique_ptr<bus::iDeviceBus>(nullptr); /* default is null */

        //std::function<int(vector<byte>&)> _readCallback;

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