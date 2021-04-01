/**
 * @file    system.hpp
 * @brief   system information functions header
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SYS_SYSTEM_HPP_
#define _OPENEDGE_SYS_SYSTEM_HPP_

namespace oe::sys {


class system {
    public:
        system() = default;
        virtual ~system() = default;
    
    static const char* get_OSversion()

};

} /* namespace */

#endif