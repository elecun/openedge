/**
 * @file    general.hpp
 * @brief   general device
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.01.27
 **/

#ifndef _OPENEDGE_DEVICE_GENERAL_HPP_
#define _OPENEDGE_DEVICE_GENERAL_HPP_

#include <openedge/device/abstract.hpp>
#include <openedge/bus/abstract.hpp>

namespace oe {

    class generalDevice : public oe::device {
        public:
            generalDevice() = default;
            generalDevice(oe::bus* bus = nullptr);
            ~generalDevice();

            int open() override;
            int close() override;
            int init() override;
            int write() override;
            int read() override;

        protected:
            oe::bus* _bus = nullptr;

    };

} /* namespace oe */

#endif