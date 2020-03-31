
/**
 * @file    info.hpp
 * @brief   system information header
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SYS_INFO_HPP_
#define _OPENEDGE_SYS_INFO_HPP_

#include <sys/sysinfo.h>


namespace oe {
    namespace sys {

        class systemInfo {
            public:
                systemInfo(){}
                ~systemInfo(){}

        }; 

    } //namespace sys
} //namespace oe

#endif