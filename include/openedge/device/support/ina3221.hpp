
/**
 * @file    ina3221.hpp
 * @brief   INA3221 device module header
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 * @date    2021.01.29
 */

#ifndef _OPENEDGE_SUPPORT_DEVICE_INA3221_HPP_
#define _OPENEDGE_SUPPORT_DEVICE_INA3221_HPP_

#include <string>
#include <openedge/device/prepheral.hpp>

using namespace std;

namespace oe { class device; }
namespace oe::support {

    class INA3221 : public oe::prepheral {
        public:
            INA3221(oe::device* device, unsigned char address);
            virtual ~INA3221();

            bool open() override;
            void close();

            unsigned short read(unsigned char address) override;

        private:
            string _dev;
            int _devfd = -1;
            unsigned char _address;

            oe::device* _device;
    };

} /* namespace */

#endif