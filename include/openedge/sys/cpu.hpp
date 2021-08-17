
/**
 * @file    CPU.hpp
 * @brief   cpu performance measure
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 * @note    It depends on Filesystem of your Operating System
 */

#ifndef _OPENEDGE_SYS_CPU_HPP_
#define _OPENEDGE_SYS_CPU_HPP_

#include <openedge/arch/singleton.hpp>
#include <fstream>
#include <stdexcept>

using namespace std;

#if defined(__linux__)
#include <sys/sysinfo.h>

namespace oe::sys {

    class cpu {
        public:
            cpu() = default;
            virtual ~cpu() = default;
            

    }; /* class */

    class cpu_perf : public cpu {

        public:
            cpu_perf(const char* procfile = "/proc/stat"):_profile(procfile){

            }
            ~cpu_perf() = default;

            int get_nprocs() { return ::get_nprocs(); } /* return number of processors */



        private:
            const char* _profile { nullptr };

        

    }; /* class */

} /* namespace */

#endif // for linux
#endif // for header