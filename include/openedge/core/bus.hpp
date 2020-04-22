
/**
 * @file    bus.hpp
 * @brief   Bus Interface Class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_BUS_HPP_
#define _OPENEDGE_CORE_BUS_HPP_

#include <openedge/core/device.hpp>
#include <functional>

namespace oe::core::bus {

    class iDeviceBus : public iDevice {
        public:
            typedef std::function<void(uint8_t*, int)> readCallbackFunc;

            iDeviceBus(readCallbackFunc func = nullptr ):callback(func) {
                this->type = DEVICE_TYPE::BUS;
            }
			virtual bool open() = 0;
			virtual void close() = 0;
			virtual int read(uint8_t* data, int len) = 0;
			virtual int write(const uint8_t* data, int len) = 0;
			virtual void flush() = 0;

        protected:
			virtual void setReadCallback(readCallbackFunc func) { callback = func; }

		protected:
			readCallbackFunc callback = nullptr;
            
    }; //class iDeviceBus


    class iDeviceBusUART : public iDeviceBus {
    public:
        /* UART Baudrate */
        enum class UART_BAUDRATE : unsigned int {
            BAUDRATE_110 = 110,
            BAUDRATE_300 = 300,
            BAUDRATE_600 = 600,
            BAUDRATE_1200 = 1200,
            BAUDRATE_2400 = 2400,
            BAUDRATE_4800 = 4800,
            BAUDRATE_9600 = 9600,
            BAUDRATE_14400 = 14400,
            BAUDRATE_19200 = 19200,
            BAUDRATE_38400 = 38400,
            BAUDRATE_56000 = 56000,
            BAUDRATE_57600 = 57600,
            BAUDRATE_115200 = 115200,
            BAUDRATE_128000 = 128000,
            BAUDRATE_230400 = 230400,
            BAUDRATE_256000 = 256000,
            BAUDRATE_250000 = 250000,
            BAUDRATE_500000 = 500000,
            BAUDRATE_750000 = 750000,
        };

        enum class UART_PARITY : int {
            NONE = 0,
            ODD,
            EVEN,
        };

        enum class UART_FLOW_CONTROL : int {
            NONE = 0,
            SOFTWARE,
            HARDWARE
        };

        enum class UART_STOPBITS : int {
            ONE = 0,
            ONE5,
            TWO,
        };

        iDeviceBusUART(UART_BAUDRATE baudrate = UART_BAUDRATE::BAUDRATE_115200,
            unsigned int databits = 8,
            UART_STOPBITS stopbits = UART_STOPBITS::ONE,
            UART_PARITY parity = UART_PARITY::NONE,
            UART_FLOW_CONTROL flowcontrol = UART_FLOW_CONTROL::NONE):
            baudrate(baudrate), databits(databits), stopbits(stopbits), parity(parity), flowcontrol(flowcontrol){ }

    protected:
        UART_BAUDRATE baudrate;
        unsigned int databits;
        UART_STOPBITS stopbits;
        UART_PARITY parity;
        UART_FLOW_CONTROL flowcontrol;
    }; //class

} //namespace oe::core

#endif