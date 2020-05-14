
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
// #include <3rdparty/jsonrpccxx/server.hpp>
// #include <3rdparty/jsonrpccxx/iclientconnector.hpp>

using namespace oe;

//This class is server and client connector at the same time.
// class serviceConnector : public jsonrpccxx::IClientConnector {
// public:
//   explicit serviceConnector(jsonrpccxx::JsonRpcServer &server) : server(server) {}
//   std::string Send(const std::string &request) override { return server.HandleRequest(request); }
// private:
//   jsonrpccxx::JsonRpcServer &server;
// };

class fenetConnectorService : public core::iService {
    public:
        fenetConnectorService();
        virtual ~fenetConnectorService();
        
        //common iservice interface
        bool initService(const char* config = nullptr) override;
        bool closeService() override;

        //services APIs
        bool test(const int& value);
        bool write(const std::string& data);

    private:
        uint16_t _invokeId { 0x0000 };
        sockpp::tcp_connector _fenetConnector;
        // shared_ptr<serviceConnector> _serviceConnector;

        string _fenet_address {""};
        int _fenet_port {0};
        unsigned long long _fenet_timeout {0}; //read timeout

}; //class

EXPORT_SERVICE_API

#endif