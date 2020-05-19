
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
#include <map>

using namespace std;

class plcGeneral : public oe::core::iService {
    public:
        plcGeneral();
        ~plcGeneral();

        //service function
        void open() override;
        void close() override;

    private:
        map<int /*slot number*/, oe::core::iDeviceBus* /*added bus i/f module */> _busSlot;

}; //class plcGeneral

EXPORT_SERVICE_API

#endif