/**
 * @file    controller.hpp
 * @brief   general controller interface
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.06.01
 **/

#ifndef _OPENEDGE_DEVICE_CONTROLLER_HPP_
#define _OPENEDGE_DEVICE_CONTROLLER_HPP_
 
namespace oe::device {
    
    class controller {

        public:
            virtual bool open() = 0;
            virtual void close() = 0;

        protected:
            int id = 0;

    };

} /* namespace oe::device */

#endif