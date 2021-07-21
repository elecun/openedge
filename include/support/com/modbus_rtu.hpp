

#ifndef _OPENEDGE_SUPPORT_COM_MODBUS_RTU_HPP_
#define _OPENEDGE_SUPPORT_COM_MODBUS_RTU_HPP_

#include <openedge/device/bus.hpp>

namespace oe::support {

class modbusRTU : public oe::device::bus {

    public:
        enum class BAUDRATE : int { 
            BAUDRATE_2400 = 0,
            BAUDRATE_9600 = 1,
            BAUDRATE_19200 = 2,
            BAUDRATE_38400 = 3,
            BAUDRATE_115200 = 4
        };

        modbusRTU();
        virtual ~modbusRTU();

        bool open() override;
        void close() override;
        int read(unsigned char* data, int len) override;
        int write(const unsigned char* data, int len) override;
        void flush() override;

};



#endif