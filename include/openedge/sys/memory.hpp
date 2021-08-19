
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

    namespace perf {
        
        class memory : public oe::sys::memory {

            public:
                memory(const char* procfile = "/proc/stat"):_profile(procfile){
                    
                }
                ~memory() = default;

                json perform(); //total memory resource measure


            private:
                const char* _profile { nullptr };

            }; /* class */

    } /* perf namespace */
} /* sys namespace */

#endif // for linux
#endif //for header