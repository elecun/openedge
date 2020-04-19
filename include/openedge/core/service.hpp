
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
    class iservice {
        virtual void open() = 0;
        virtual void close() = 0;
    }; //class iservice

    typedef oe::core::iservice*(*create_service)(void);
    typedef void(*release_service)(void);

    #define EXPORT_SERVICE_API extern "C" { oe::core::iservice* create(void); void release(void); }

} //namespace service


#endif