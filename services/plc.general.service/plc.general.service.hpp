
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

    class plcGeneral : public oe::core::iDeviceExtend, oe::core::iservice {
        public:
            plcGeneral();
            ~plcGeneral();

            //service function
            void open() override;
            void close() override;

            bool addModule(_mtype* module) = 0;
            bool removeModule(_mod_index) = 0;
            _mtype* getModule(_mod_index) = 0;

        private:
            oe::core::ibusAsync* _bus;

    };
    
} //namespace os::service

#endif