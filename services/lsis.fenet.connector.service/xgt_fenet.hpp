

/**
 * @file    xgt_fenet.hpp
 * @brief   LSIS XGB Fast Ethernet Concrete Impl.
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_XGB_FENT_HPP_
#define _OPENEDGE_XGB_FENT_HPP_

#include <openedge/core/bus.hpp>
#include <vector>

using namespace std;

class XGBFastEthernet : public oe::core::ibusAsync {
    public:
        XGBFastEthernet();
        XGBFastEthernet(const char* ip4v_addr, int port);

        bool open() override;
        void close() override;
        int read(uint8_t* data, int len) override;
        int write(const uint8_t* data, int len) override;
        void flush() override;

    private:
        vector<uint8_t> command(vector<uint8_t>& cmd);

}; //class XGBFastEthernet



#endif 