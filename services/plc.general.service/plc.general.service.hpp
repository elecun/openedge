
/**
 * @file    plc.service.hpp
 * @brief   PLC Service Header File
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_PLC_GENERAL_HPP_
#define _OPENEDGE_SERVICE_PLC_GENERAL_HPP_

#include <openedge/core/device.hpp>
#include <openedge/core/bus.hpp>
#include <openedge/core/service.hpp>

namespace oe::service {

    class plcGeneral : public oe::core::idevice, oe::core::iservice {
        public:
            plcGeneral();
            ~plcGeneral();

            //service function
            void open() override;
            void close() override;

        private:
            oe::core::ibusAsync* _bus;

    };
    
} //namespace os::service

#endif