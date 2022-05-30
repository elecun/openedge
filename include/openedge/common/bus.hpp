
/**
 * @file    bus.hpp
 * @brief   Bus Interface Class (Sync & Async)
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_INTERFACE_BUS_HPP_
#define _OPENEDGE_INTERFACE_BUS_HPP_

#include <openedge/common/device.hpp>
#include <functional>

namespace oe::interface {

    class syncBus : public interface::device {
        public:
            syncBus() {
                this->type = dType::BUS;
            }
			virtual bool open() = 0;
			virtual void close() = 0;
			virtual int read(uint8_t* data, int len) = 0;   //read block
            virtual int read_until(uint8_t* data, int len, unsigned int t_ms) = 0;  //read block with timeout
			virtual int write(const uint8_t* data, int len) = 0;
            
    }; //bus interface class

    class asyncBus : public interface::device {
        typedef std::function<void(uint8_t*, int)> readcallback;
        public:
            asyncBus(){}

            /**
             * @brief Set the read callback function pointer
             * 
             * @param func function pointer
             */
			virtual void set_read_callback(readcallback func) { pfCallback = func; }

        protected:
			readcallback pfCallback = nullptr;

    }; //class async bus

    class busUART {
    public:
        /* UART Baudrate */
        enum class busBaudrate : unsigned int {
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

        enum class busParity : int {
            NONE = 0,
            ODD,
            EVEN,
        };

        enum class busFlowControl : int {
            NONE = 0,
            SOFTWARE,
            HARDWARE
        };

        enum class busStopbits : int {
            ONE = 0,
            ONE5,
            TWO,
        };

        busUART(busBaudrate baudrate = busBaudrate::BAUDRATE_115200,
            unsigned int databits = 8,
            busStopbits stopbits = busStopbits::ONE,
            busParity parity = busParity::NONE,
            busFlowControl flowcontrol = busFlowControl::NONE):
            baudrate(baudrate), databits(databits), stopbits(stopbits), parity(parity), flowcontrol(flowcontrol){ }

    protected:
        busBaudrate baudrate;
        unsigned int databits;
        busStopbits stopbits;
        busParity parity;
        busFlowControl flowcontrol;

    }; //class busUART

} //namespace oe::interface

#endif