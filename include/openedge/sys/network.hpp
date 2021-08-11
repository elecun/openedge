
/**
 * @file    network.hpp
 * @brief   network load measurement
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 * @note    It depends on Filesystem of your Operating System
 */

#ifndef _OPENEDGE_SYS_NETWORK_HPP_
#define _OPENEDGE_SYS_NETWORK_HPP_

#include <string>
#include <openedge/arch/singleton.hpp>
#include <3rdparty/json.hpp>

using namespace std;
using json = nlohmann::json;


/* class for network performance measure*/
namespace oe::sys::perf {

    #define NET_STATFILE    "/proc/net/dev"

    class network : oe::arch::singleton<network> {
        public:
            explicit network(const char* interface);

            const char* getInterfaceName() const { return _interface.c_str(); }


        private:
            string _interface { "eth0" };
    };

} /* namespace */

#define openedge_network oe::sys::perf::network::instance()

#endif