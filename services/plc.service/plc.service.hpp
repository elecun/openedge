
/**
 * @file    plc.service.hpp
 * @brief   PLC Service Header File
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_PLC_HPP_
#define _OPENEDGE_SERVICE_PLC_HPP_

#include <openedge/core/device.hpp>
#include <openedge/core/bus.hpp>

namespace oe::service {

    class plc : public oe::core::idevice, oe::core::iservice{
        public:
            plc(oe::core::ibusAsync* bus){}
            ~plc() {}


    };
    
} //namespace os::service

#endif