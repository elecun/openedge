/**
 * @file    abstract.hpp
 * @brief   bus inteface
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.01.27
 **/

#ifndef _OPENEDGE_BUS_ABSTRACT_HPP_
#define _OPENEDGE_BUS_ABSTRACT_HPP_
 
namespace oe {
    
    class bus {
        public:
            virtual ~bus() {}
    };

} /* namespace oe */

#endif