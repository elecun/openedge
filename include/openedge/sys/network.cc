
#include "network.hpp"
#include <openedge/log.hpp>
#include <fstream>

namespace oe::sys {
    network::network(const char* interface)
    :_interface(interface) {

    }

    string network::getMACAddress(){
        ifstream in(fmt::format("/sys/class/net/{}/address", _interface).c_str());
        string out { "unknown "};
        if(in.is_open())
            in>>out;
        in.close();
        return out;
    }

}