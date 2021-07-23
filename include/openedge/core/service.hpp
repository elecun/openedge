
/**
 * @file    service.hpp
 * @brief   Service Interface Class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_SERVICE_HPP_
#define _OPENEDGE_CORE_SERVICE_HPP_

#include <string>
#include <map>
#include <type_traits>
#include <3rdparty/jsonrpccxx/server.hpp>
#include <3rdparty/json.hpp>

using namespace std;
using namespace jsonrpccxx;
using json = nlohmann::json;



/* for local service interface definition */
namespace oe::core {
    class service {
        public:
            virtual bool init() = 0;
            virtual bool request() = 0;
            virtual bool valid() = 0;

    }; /* service class */
} /* namespace */

/* for RPC-based service interface definition */
namespace oe {
    namespace core {

        class iService {
            public:
                iService():service(make_unique<jsonrpccxx::JsonRpc2Server>()){}
                virtual ~iService() {}
                virtual bool initService(const char* config) = 0; //config is dumped json from profile
                virtual bool closeService() = 0;
                virtual string request(const string& callmsg) {
                    if(service){
                        return service->HandleRequest(callmsg);
                    }
                    return string("");
                }
                virtual jsonrpccxx::JsonRpc2Server& getServicePort() const { return *this->service.get(); }
            protected:
                shared_ptr<jsonrpccxx::JsonRpc2Server> service;
                json serviceConfig;
        }; //class iService

    } //namespace core

    typedef oe::core::iService*(*create_service)(void);
    typedef void(*release_service)(void);

    #define EXPORT_SERVICE_API extern "C" {oe::core::iService* create(void); void release(void);}
} //namespace service


#endif