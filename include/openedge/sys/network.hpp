
/**
 * @file    network.hpp
 * @brief   network load measurement
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 * @note    It depends on Filesystem of your Operating System
 */

#ifndef _OPENEDGE_SYS_NETWORK_HPP_
#define _OPENEDGE_SYS_NETWORK_HPP_

#include <openedge/arch/singleton.hpp>
#include <3rdparty/json.hpp>
#include <string>
#include <map>

using namespace std;
using json = nlohmann::json;

#if defined(__linux__)
#include <sys/sysinfo.h>

namespace oe::sys {

    class network {
        public:
            explicit network(const char* interface);
            virtual ~network() = default;

            const char* getInterfaceName() const { return _interface.c_str(); }     //get network interface use
            string getMACAddress();     // get MAC address of network interface use

        private:
            string _interface { "eth0" };
            

    }; /* class */

    namespace stat{

        class network : public oe::sys::network {
            public:
                explicit network(const char* netif = "eth0", const char* procfile = "/proc/net/dev");
                ~network() = default;

            json stats();     //network resource measure

            unsigned long getTXbps();       //get Transferred Bytes per second
            unsigned long getTXbps_boot();  //get Transferred Bytes per second since startup
            unsigned long getRXbps();       //get Recieved Bytes per second
            unsigned long getRXbps_boot();  //get Transferred Bytes per second since startup

            private:
                const char* _procfile { nullptr };
                std::map<std::string, std::string> _netstat;

        }; /* class */
    }

} /* namespace */

#define this_network oe::sys::perf::network::instance()

#endif // for linux
#endif //for header