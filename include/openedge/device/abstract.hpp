/**
 * @file    abstract.hpp
 * @brief   device interface class
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.01.27
 **/

#ifndef _OPENEDGE_DEVICE_ABSTRACT_HPP_
#define _OPENEDGE_DEVICE_ABSTRACT_HPP_
 
namespace oe {
    
    class device {
        public:
            virtual ~device() {}

            virtual int open() = 0;
            virtual int close() = 0;
            virtual int init() = 0;
            virtual int write() = 0;
            virtual int read() = 0;
    };

} /* namespace oe */

#endif