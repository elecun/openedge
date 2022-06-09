/**
 * @file aop.h2o2gen.emg.p1.task.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Task for AOP H2O2 Generator Emergency(H2O2 Temperatuire) Check
 * @version 0.1
 * @date 2022-06-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_AOP_H2O2GEN_EMERGENCY_P1_TASK_HPP_
#define _OPENEDGE_AOP_H2O2GEN_EMERGENCY_P1_TASK_HPP_

#include <openedge/core.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

using namespace oe;

class aop_h2o2gen_emg_p1_task : public oe::core::task::runnable_rt {
    public:
        aop_h2o2gen_emg_p1_task();
        ~aop_h2o2gen_emg_p1_task();

        /* basic interface functions */
        virtual void execute() override;
        virtual void stop() override;
        virtual bool configure() override;
        virtual void cleanup() override;
        virtual void pause() override;
        virtual void resume() override;

    private:
        double _limit_temperature = 0.0;

}; /* end class */

EXPORT_TASK_API

#endif