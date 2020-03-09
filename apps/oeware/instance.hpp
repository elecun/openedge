
/**
 * @file    instance.hpp
 * @brief   oeware global instance
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */


#ifndef _OEWARE_INSTANCE_HPP_
#define _OEWARE_INSTANCE_HPP_


namespace oe {
    namespace ware {

        extern bool init(const char* config_file);
        extern void start(void);

    } //namespace ware
} //namespace oe


#endif