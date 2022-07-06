/**
 * @file    instance.hpp
 * @brief   Openedge Engine Header
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _EDGE_INSTANCE_HPP_
#define _EDGE_INSTANCE_HPP_

namespace oe::app {

    //application initialize
    bool initialize(const char* conf_file);

    //app start to run
    void run();

    //app cleanup
    void cleanup();

}

// namespace oe::manager {

//     /**
//      * @brief   initialize process manager
//      */
//     bool initialize_proc();

//     /**
//      * @brief   start process manager
//      */
//     void run();

//     /**
//      * @brief   terminate process manager
//      */
//     void cleanup();
// }

#endif