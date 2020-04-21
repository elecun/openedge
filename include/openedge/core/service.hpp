
/**
 * @file    service.hpp
 * @brief   Service Interface Class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_SERVICE_HPP_
#define _OPENEDGE_CORE_SERVICE_HPP_

#include <string>

using namespace std;

namespace oe::core {

    class iService {
        virtual void open() = 0;
        virtual void close() = 0;
    }; //class iService

    typedef oe::core::iService*(*create_service)(void);
    typedef void(*release_service)(void);

    #define EXPORT_SERVICE_API extern "C" { oe::core::iService* create(void); void release(void); }

} //namespace service


#endif