/**
 * @file aop.h2o2gen.plc.connector.task.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief PLC Connection & Data Management Component
 * @version 0.1
 * @date 2022-06-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_AOP_H2O2GEN_PLC_CONNECTOR_TASK_HPP_
#define _OPENEDGE_AOP_H2O2GEN_PLC_CONNECTOR_TASK_HPP_

#include <openedge/core.hpp>

using namespace oe;

class aop_h2o2gen_plc_connector_task : public oe::core::task::runnable_nt {
    public:
        aop_h2o2gen_plc_connector_task();
        ~aop_h2o2gen_plc_connector_task();

        /* basic interface functions */
        virtual void execute() override;
        virtual void stop() override;
        virtual bool configure() override;
        virtual void cleanup() override;
        virtual void pause() override;
        virtual void resume() override;


}; /* end class */

EXPORT_TASK_API

#endif