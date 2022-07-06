/**
 * @file    instance.hpp
 * @brief   Openedge Engine Header
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _EDGE_INSTANCE_HPP_
#define _EDGE_INSTANCE_HPP_

namespace oe::app {

    bool initialize(const char* conf_file);
    void run();
    void cleanup();

}

#endif