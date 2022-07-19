/**
 * @file device.iologik.e1212.dio.hpp
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief MOXA iologik E1212 Analog Input Device Component
 * @version 0.1
 * @date 2022-07-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_DEVICE_IOLOGIK_E1212_DIO_HPP_
#define _OPENEDGE_DEVICE_IOLOGIK_E1212_DIO_HPP_

#include <openedge/core.hpp>
#include <openedge/common/bus.hpp>
#include <memory>

using namespace oe;
using namespace std;

class device_iologik_e1212_dio : public core::task::runnable_nt {
    public:
        device_iologik_e1212_dio();
        ~device_iologik_e1212_dio();

        /* basic interface functions for nt */
        virtual void execute() override;
        virtual void stop() override;
        virtual bool configure() override;
        virtual void cleanup() override;
        virtual void pause() override;
        virtual void resume() override;

        virtual void on_request(std::any data) override;

    private:
        bus::sync_bus* _device = nullptr;   //serial device
        double _t_out = 0.5;    //0.5 sec
        

}; /* end class */

EXPORT_TASK_API

#endif