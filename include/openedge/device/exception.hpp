/**
 * @file    exception.hpp
 * @brief   device exception
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.01.27
 **/

#ifndef _OPENEDGE_DEVICE_SETUPEXCEPTION_HPP_
#define _OPENEDGE_DEVICE_SETUPEXCEPTION_HPP_

#include <exception>
#include <string>

using namespace std;

namespace oe {
    namespace device {

        class setupException : public exception {
            public:
                setupException(const char* msg):_errmsg(msg){

                }
                const char* waht() const throw() { return _errmsg.c_str(); }

            private:
                string _errmsg;
        };

    } /* namespace device */
} /* namespace oe */

#endif