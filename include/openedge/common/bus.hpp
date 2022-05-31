
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

    class sync_bus : public interface::device {
        public:
            sync_bus() {
                this->type = type_d::BUS;
            }
			virtual bool open() = 0;
			virtual void close() = 0;
			virtual int read(uint8_t* data, int len) = 0;   //read block
            virtual int read_until(uint8_t* data, int len, unsigned int t_ms) = 0;  //read block with timeout
			virtual int write(const uint8_t* data, int len) = 0;
            
    }; //bus interface class

    class async_bus : public interface::device {
        typedef std::function<void(uint8_t*, int)> readcallback_d;
        public:
            async_bus(){}

            /**
             * @brief Set the read callback function pointer
             * 
             * @param func function pointer
             */
			virtual void set_read_callback(readcallback_d func) { pfCallback = func; }

        protected:
			readcallback_d pfCallback = nullptr;

    }; //class async bus

    class bus_uart {
    public:
        /* UART Baudrate */
        enum class baudrate_d : unsigned int {
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

        enum class parity_d : int {
            NONE = 0,
            ODD,
            EVEN,
        };

        enum class flowcontrol_d : int {
            NONE = 0,
            SOFTWARE,
            HARDWARE
        };

        enum class stopbits_d : int {
            ONE = 0,
            ONE5,
            TWO,
        };

        bus_uart(baudrate_d baudrate = baudrate_d::BAUDRATE_115200,
            unsigned int databits = 8,
            stopbits_d stopbits = stopbits_d::ONE,
            parity_d parity = parity_d::NONE,
            flowcontrol_d flowcontrol = flowcontrol_d::NONE):
            baudrate(baudrate), databits(databits), stopbits(stopbits), parity(parity), flowcontrol(flowcontrol){ }

    protected:
        baudrate_d baudrate;
        unsigned int databits;
        stopbits_d stopbits;
        parity_d parity;
        flowcontrol_d flowcontrol;

    }; //class bus_uart

} //namespace oe::interface

#endif