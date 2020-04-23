
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

        class iServiceBase { };

        template<class _T>
        class iService : public iServiceBase {
            public:
                virtual void open() = 0;
                virtual void close() = 0;

            protected:
                _T* instance = nullptr;
        }; //class iService

        // namespace service {
        //     //template<class _ServiceClassT, typename std::enable_if_t<std::is_class_v<_ServiceClassT>, _ServiceClassT>* = nullptr>
        //     template<class _ServiceClassT, typename std::enable_if_t<std::is_same_v<_ServiceClassT, iServiceBase>, _ServiceClassT>* = nullptr>
        //     class container {
        //         public:
        //             serviceManager() { }

        //         protected:
        //             std::map<unsigned int, _ServiceClassT*> serviceMap;
        //     };
        // } //namespace service

    } //namespace core

    // typedef oe::core::iServiceBase*(*create_service)(void);
    // typedef void(*release_service)(void);

    // #define EXPORT_SERVICE_API extern "C" { oe::core::iServiceBase* create(void); void release(void); }
} //namespace service


#endif