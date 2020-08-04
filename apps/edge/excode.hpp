

/**
 * @file    excode.hpp
 * @brief   exception code for edge application
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */


namespace oe {
    namespace edge {
        enum class excode : int {
        EDGE_LOAD_CONFIG_FAIL = 100,      //if configuration file loads failed
    };

    } //namespace edge
} //namespace oe