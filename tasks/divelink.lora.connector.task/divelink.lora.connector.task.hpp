/**
 * @file divelink.lora.connector.task.hpp
 * @author Byunghun Hwang *bh.hwang@iae.re.kr)
 * @brief LoRa Communication task
 * @version 0.1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_DIVELINK_LORA_CONNECTOR_TASK_HPP_
#define _OPENEDGE_DIVELINK_LORA_CONNECTOR_TASK_HPP_

#include <openedge/core.hpp>
#include <openedge/common/bus.hpp>
#include <memory>

using namespace oe;
using namespace std;

class divelink_lora_connector_task : public core::task::runnable_nt {
    public:
        divelink_lora_connector_task();
        ~divelink_lora_connector_task();

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
        double _t_out = 0.5;
        

}; /* end class */

EXPORT_TASK_API

#endif