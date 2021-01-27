/**
 * @file    CJMCU3221.hpp
 * @brief   INA3221-based CJMCU-3221 device module library
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.01.27
 **/

#ifndef _OPENEDGE_DEVICE_CJMCU3221_HPP_
#define _OPENEDGE_DEVICE_CJMCU3221_HPP_

#include <openedge/device/abstract.hpp>

namespace oe {

    class cjmcu3221 : public oe::device {
        public:
            cjmcu3221() = default;
            ~cjmcu3221();

            int open() override;
            int close() override;
            int init() override;
            int write() override;
            int read() override;

        protected:

    };

} /* namespace oe */

#endif