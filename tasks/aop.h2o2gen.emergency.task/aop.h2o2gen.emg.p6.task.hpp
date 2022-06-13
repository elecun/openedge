/**
 * @file aop.h2o2gen.emg.p1.task.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Task for AOP H2O2 Generator Emergency(H2O2 rectifier maximum voltage/current) Check
 * @version 0.1
 * @date 2022-06-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_AOP_H2O2GEN_EMERGENCY_P6_TASK_HPP_
#define _OPENEDGE_AOP_H2O2GEN_EMERGENCY_P6_TASK_HPP_

#include <openedge/core.hpp>

using namespace oe;

class aop_h2o2gen_emg_p6_task : public oe::core::task::runnable_rt {
    public:
        aop_h2o2gen_emg_p6_task();
        ~aop_h2o2gen_emg_p6_task();

        /* basic interface functions */
        virtual void execute() override;
        virtual void stop() override;
        virtual bool configure() override;
        virtual void cleanup() override;
        virtual void pause() override;
        virtual void resume() override;

    private:
        double _max_voltage = 0.0;
        double _max_current = 0.0;
        double _duration = 10; //sec

}; /* end class */

EXPORT_TASK_API

#endif