/**
 * @file    device.hpp
 * @brief   general device
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.01.27
 **/

#ifndef _OPENEDGE_DEVICE_GENERAL_HPP_
#define _OPENEDGE_DEVICE_GENERAL_HPP_

#include <vector>

using namespace std;

namespace oe::general {

        class device : public oe::device {
        public:
            device() = default;
            device(oe::bus* bus = nullptr);
            virtual ~device();

            int open() override;
            int close() override;
            int init() override;
            int write() override;
            int read() override;

            oe::bus* getBus() { return _bus; }

        private:
            

        protected:
            oe::bus* _bus = nullptr;

    };

} /* namespace */

#endif