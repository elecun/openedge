
/**
 * @file    logger.task.hpp
 * @brief   Sensor Data Logger with I2C Interface
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_LOGGER_TASK_HPP_
#define _OPENEDGE_LOGGER_TASK_HPP_

#include <openedge/core.hpp>
#include <vector>
#include <map>
#include <tuple>
#include <fstream>

using namespace oe;
using namespace std;


namespace oe{ class device; }
class loggerTask : public oe::core::task::runnable {
    public:
        loggerTask() = default;
        virtual ~loggerTask() = default;

        //common task interfaces
        bool configure() override;
        void execute() override;
        void cleanup() override;

    private:
        ofstream _logfile;
        oe::device* _device = nullptr;  //prepheral device to use


EXPORT_TASK_API

#endif


