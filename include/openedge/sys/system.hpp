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
#include "network.hpp"  //for network performance measure
#include "memory.hpp"   //for memory performance measure
#include "process.hpp"  //for specific process measure

using namespace std;
using json = nlohmann::json;

namespace oe::sys {

class system : public oe::arch::singleton<system> {

    friend class oe::sys::perf::cpu; 
    friend class oe::sys::perf::network;
    friend class oe::sys::perf::memory;
    friend class oe::sys::perf::process;

    public:
        system(){
            _net = make_unique<perf::network>("eth0", "/proc/net/dev");
        }
        virtual ~system() = default;

        /* return system information */
        json getInfo();

        /* return system resources use */
        json getCurrentResources();

        /* get os version information */
        string get_os_version();

        private:
            //cpu_perf _cpu_perf;
            //memory_perf _mem_perf;
            unique_ptr<perf::network> _network;
            unique_ptr<perf::cpu> _cpu;
            unique_ptr<perf::memory> _memory;
            unique_ptr<perf::process> _process;



}; /* class */

} /* namespace */

#define this_system  oe::sys::system::instance()

#endif /* linux system */
#endif /* end header */