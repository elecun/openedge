/**
 * @file    system.hpp
 * @brief   system information & performance measure
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SYS_SYSTEM_HPP_
#define _OPENEDGE_SYS_SYSTEM_HPP_

#if defined(__linux__)

#include <openedge/arch/singleton.hpp>
#include <3rdparty/json.hpp>
#include <fstream>

using namespace std;
using json = nlohmann::json;

namespace oe::sys {

class system : public oe::arch::singleton<system> {
    public:
        system() = default;
        virtual ~system() = default;

        /* return summarized measures */
        json summarize(){

        }

        /* get os version information */
        string get_os_version();



}; /* class */

} /* namespace */

#define this_system  oe::sys::system::instance()

#endif /* linux system */
#endif /* end header */