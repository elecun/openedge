
/**
 * @file    memload.hpp
 * @brief   memory load measurement
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 * @note    It depends on Filesystem of your Operating System
 */

#ifndef _OPENEDGE_SYS_MEMLOAD_HPP_
#define _OPENEDGE_SYS_MEMLOAD_HPP_

#include <openedge/arch/singleton.hpp>

#if defined(__linux__)
#include <sys/sysinfo.h>

namespace oe::sys {

    class memory {
        public:
            memory() = default;
            virtual ~memory() = default;
            

    }; /* class */

    class memory_perf : public memory {

        public:
            memory_perf(const char* procfile = "/proc/stat"):_profile(procfile){
                
            }
            ~memory_perf() = default;


        private:
            const char* _profile { nullptr };

        

    }; /* class */

} /* namespace */

#endif // for linux
#endif //for header