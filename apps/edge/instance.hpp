/**
 * @file    instance.hpp
 * @brief   Openedge Engine Header
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _EDGE_INSTANCE_HPP_
#define _EDGE_INSTANCE_HPP_

namespace oe {
    namespace edge {

        //initialize
        bool init(const char* conf_file);

        //start edge
        void run();


    } //namespace edge

} //namespace oe

#endif