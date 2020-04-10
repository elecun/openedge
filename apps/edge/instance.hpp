/**
 * @file    instance.hpp
 * @brief   Openedge Engine Header
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _EDGE_INSTANCE_HPP_
#define _EDGE_INSTANCE_HPP_

namespace oe {
    namespace edge {

        //initialize edge application
        bool init(const char* conf_file);

        //start to run
        void run();

        //cleanup
        void cleanup();


    } //namespace edge

} //namespace oe

#endif