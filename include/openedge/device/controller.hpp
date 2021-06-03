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
            controller(int cid = 1):id(cid){ 

            }
            virtual ~controller() = default;
            
            virtual bool open() = 0;
            virtual void close() = 0;

            const int get_id() { return id; }

        private:
            int id = 1;

    };

} /* namespace oe::device */

#endif