/**
 * @file    prepheral.hpp
 * @brief   prepheral device class (general)
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.01.27
 **/

#ifndef _OPENEDGE_DEVICE_PREPHERAL_HPP_
#define _OPENEDGE_DEVICE_PREPHERAL_HPP_

#include <string>

using namespace std;
 
namespace oe {
    class device;
    enum class prepheral_class : int {
        I2C = 100,
    };
    
    class prepheral {
        public:
            prepheral() { }
            virtual ~prepheral() { }

            virtual bool open() = 0;
            virtual unsigned short read(unsigned char address) = 0;
    };

} /* namespace oe */

#endif