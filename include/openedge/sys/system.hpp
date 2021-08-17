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

#include "cpu.hpp"  // for CPU performance measure
#include "network.hpp"  //for network performance measure
#include "memory.hpp"   //for memory performance measure

using namespace std;
using json = nlohmann::json;

namespace oe::sys {

class system : public oe::arch::singleton<system> {

    //friend class oe::sys::perf::cpu_perf; 
    friend class oe::sys::perf::network_perf;
    //friend class oe::sys::perf::memory_perf;

    public:
        system(){
            _net_perf = make_unique<perf::network_perf>("eth0", "/proc/net/dev");
        }
        virtual ~system() = default;

        /* return summarized measures */
        json summarize();

        /* get os version information */
        string get_os_version();

        private:
            //cpu_perf _cpu_perf;
            //memory_perf _mem_perf;
            unique_ptr<perf::network_perf> _net_perf;



}; /* class */

} /* namespace */

#define this_system  oe::sys::system::instance()

#endif /* linux system */
#endif /* end header */