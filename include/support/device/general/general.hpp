/**
 * @file    general.hpp
 * @brief   general device
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.01.27
 **/

#ifndef _OPENEDGE_DEVICE_GENERAL_HPP_
#define _OPENEDGE_DEVICE_GENERAL_HPP_

#include <openedge/device/abstract.hpp>
#include <openedge/device/busi2c.hpp>
#include <vector>

using namespace std;

namespace oe {

    class generalDevice : public oe::device {
        public:
            generalDevice() = default;
            generalDevice(oe::bus* bus = nullptr);
            virtual ~generalDevice();

            int open() override;
            int close() override;
            int init() override;
            int write() override;
            int read() override;

            oe::busI2C* getBusI2C() { return _busi2c; }

        private:
            

        protected:
            oe::busI2C* _busi2c = nullptr;

    };

} /* namespace oe */

#endif