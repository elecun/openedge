/**
 * @file device.nport5232.converter.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief MOXA NPort5232 Ethernet to RS232/485 Converter
 * @version 0.1
 * @date 2022-07-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_DEVICE_NPORT5232_ADAPTER_HPP_
#define _OPENEDGE_DEVICE_NPORT5232_ADAPTER_HPP_

#include <openedge/core.hpp>
#include <openedge/common/bus.hpp>
#include <openedge/device/controller.hpp>
#include <memory>

using namespace oe;
using namespace std;

class device_nport5232_adapter : public core::task::runnable_nt {
    public:
        device_nport5232_adapter();
        ~device_nport5232_adapter();

        /* basic interface functions for nt */
        virtual void execute() override;
        virtual void stop() override;
        virtual bool configure() override;
        virtual void cleanup() override;
        virtual void pause() override;
        virtual void resume() override;

        virtual void on_request(std::any data) override;

    private:
        device::controller* _controller = nullptr;

    private:
        bus::sync_bus* _device = nullptr;   //serial device
        

}; /* end class */

EXPORT_TASK_API

#endif