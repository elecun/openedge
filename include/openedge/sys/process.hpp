
/**
 * @file    process.hpp
 * @brief   process resources
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 * @note    It depends on Filesystem of your Operating System
 */

#ifndef _OPENEDGE_SYS_PROCESS_HPP_
#define _OPENEDGE_SYS_PROCESS_HPP_

#include <openedge/arch/singleton.hpp>
#include <3rdparty/json.hpp>
#include <string>
#include <map>

using namespace std;
using json = nlohmann::json;

#if defined(__linux__)
#include <sys/sysinfo.h>

namespace oe::sys {

    class process {
        public:
            explicit process(const char* interface);
            virtual ~process() = default;
           

    }; /* class */

    namespace stat {

        class process : public oe::sys::process {
            public:
                explicit process();
                ~process() = default;

            json stats(const char* proc);   /* get statistics on specific proc status */

        }; /* class */
    }

} /* namespace */

#define this_network oe::sys::perf::network::instance()

#endif // for linux
#endif //for header