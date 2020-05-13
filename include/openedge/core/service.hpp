
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

using namespace std;

namespace oe {
    namespace core {

        class iService {
            public:
                virtual ~iService() {}
                virtual bool initService(const char* config) = 0; //config is dumped json from profile
                virtual bool closeService() = 0;
        }; //class iService

    } //namespace core

    typedef oe::core::iService*(*create_service)(void);
    typedef void(*release_service)(void);

    #define EXPORT_SERVICE_API extern "C" {oe::core::iService* create(void); void release(void);}
} //namespace service


#endif