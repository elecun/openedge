
/**
 * @file    logger.task.hpp
 * @brief   Sensor Data Logger with I2C Interface
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_LOGGER_TASK_HPP_
#define _OPENEDGE_LOGGER_TASK_HPP_

#include <openedge/core.hpp>
#include <fstream>
#include <string>
#include <time.h>

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
        void pause() override;
        void resume() override;

    private:
        void measure();   

    private:
        struct timespec x; 
        ofstream _logfile;
        int _fd = -1;
        string _devname;
        //unsigned char _i2c_address[2] = {0x40, 0x41};
        //unsigned char _registers[3] = {0x02, 0x04, 0x06}; //channle 1,2,3 bus voltages

};

EXPORT_TASK_API

#endif


