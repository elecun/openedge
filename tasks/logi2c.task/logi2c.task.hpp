
/**
 * @file    logi2c.task.hpp
 * @brief   Sensor Data Logger with I2C Interface
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_LOGI2C_TASK_HPP_
#define _OPENEDGE_LOGI2C_TASK_HPP_

#include <openedge/core.hpp>

using namespace oe;
using namespace std;

class logi2cTask : public oe::core::task::runnable {
    public:
        logi2cTask() = default;
        virtual ~logi2cTask() = default;

        //common task interfaces
        bool configure() override;
        void execute() override;
        void cleanup() override;        
};

EXPORT_TASK_API

#endif


