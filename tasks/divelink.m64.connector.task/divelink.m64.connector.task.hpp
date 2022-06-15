/**
 * @file divelink.m64.control.task.hpp
 * @author Byunghun Hwang *bh.hwang@iae.re.kr)
 * @brief Waterlink M64 Acoustic Modem Control Task for Divelink
 * @version 0.1
 * @date 2022-06-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_DIVELINK_M64_CONTROL_TASK_HPP_
#define _OPENEDGE_DIVELINK_M64_CONTROL_TASK_HPP_

#include <openedge/core.hpp>

using namespace oe;

class divelink_m64_control_task : public oe::core::task::runnable_nt {
    public:
        divelink_m64_control_task();
        ~divelink_m64_control_task();

        /* basic interface functions */
        virtual void execute() override;
        virtual void stop() override;
        virtual bool configure() override;
        virtual void cleanup() override;
        virtual void pause() override;
        virtual void resume() override;

        virtual void on_request(std::any data) override;

}; /* end class */

EXPORT_TASK_API

#endif