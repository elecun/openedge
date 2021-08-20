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
#include <memory>

#include "cpu.hpp"      // for CPU performance measure
#include "network.hpp"  // for network performance measure
#include "memory.hpp"   // for memory performance measure
#include "process.hpp"  // for specific process measure

using namespace std;
using json = nlohmann::json;

namespace oe::sys {

class system : public oe::arch::singleton<system> {

    friend class oe::sys::stat::cpu; 
    friend class oe::sys::stat::network;
    friend class oe::sys::stat::memory;
    friend class oe::sys::stat::process;

    public:
        system(){
            _netstat = make_unique<stat::network>("eth0", "/proc/net/dev");
        }
        virtual ~system() = default;

        json getInfo();     /* getting system general information */
        json getStats();    /* getting system statistics of resources */

        private:
            unique_ptr<stat::network> _netstat;
            unique_ptr<stat::cpu> _cpustat;
            unique_ptr<stat::memory> _memstat;
            unique_ptr<stat::process> _procstat;



}; /* class */

} /* namespace */

#define this_system  oe::sys::system::instance()

#endif /* linux system */
#endif /* end header */