
/**
 * @file    ina3221.hpp
 * @brief   INA3221 device module header
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 * @date    2021.01.29
 */

#ifndef _OPENEDGE_SUPPORT_DEVICE_INA3221_HPP_
#define _OPENEDGE_SUPPORT_DEVICE_INA3221_HPP_

#include <string>

using namespace std;

namespace oe {

    class INA3221 {
        public:

            INA3221(const char* dev, unsigned char address);   //dev : "/dev/i2c-2"
            virtual ~INA3221();

            bool open();
            void close();

        private:
            string _dev;
            int _devfd = -1;
            unsigned char _address;
    };

} /* namespace */

#endif