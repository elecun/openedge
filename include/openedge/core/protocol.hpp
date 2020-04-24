
/**
 * @file    protocol.hpp
 * @brief   Protocol Interface Class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_PROTOCOL_HPP_
#define _OPENEDGE_CORE_PROTOCOL_HPP_

#include <string>
#include <type_traits>
#include <array>
#include <vector>

using namespace std;

namespace oe::core {
    class iProtocol {
        public:
    };

    class iProtocolRaw : public iProtocol {
        public:
            
            unsigned int getSize() const { return frame.size(); }
            virtual vector<uint8_t> getFrame() { return frame; }

            virtual vector<uint8_t> getHeader() = 0;
        protected:
            vector<uint8_t> frame;
    };
}


#endif