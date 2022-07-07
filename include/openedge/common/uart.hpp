
/**
 * @file    uart.hpp
 * @brief   UART Bus Concrete Class *sync, async)
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_BUS_UART_HPP_
#define _OPENEDGE_BUS_UART_HPP_

#include <openedge/common/bus.hpp>
#include <openedge/log.hpp>
#include <functional>
#include <string>

#if defined(__linux__) || defined(__APPLE__)
    #include <termios.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <strings.h>
#endif


using namespace std;
using namespace oe;

namespace oe::bus {


    /**
     * @brief Timer class to check time out
     * 
     */
    class timeout {
        public:
            timeout(){
                this->reset();
            }

            /**
             * @brief timer initialize or reset
             * 
             */
            void reset() {
                #if defined(__linux__) || defined(__APPLE__)
                gettimeofday(&_prev_t, nullptr);
                #elif
                console::warn("Not supported yet.");
                #endif
            }

            /**
             * @brief return the elapsed time since initialization
             * 
             * @return unsigned long 
             */
            unsigned long elapsed_ms(){
                #if defined(__linux__) || defined(__APPLE__)
                struct timeval _curtime_t;
                gettimeofday(&_curtime_t, nullptr);
                
                int _sec = _curtime_t.tv_sec-_prev_t.tv_sec;
                int _usec = _curtime_t.tv_usec-_prev_t.tv_usec;
                if(_usec<0){
                    _usec = 1000000 - _prev_t.tv_usec + _curtime_t.tv_usec;
                    _sec--;
                }

                return _sec*1000 + _usec/1000;
                #elif
                console::warn("Not supported yet.");
                return 0;
                #endif
            }

        private:               
            struct timeval _prev_t;

    }; /* timeout class */

    /**
     * @brief synchronous UART communication class
     * 
     */
    class sync_uart : public bus::uart, public bus::sync_bus {
        public:
            // sync_uart(const char* dev, 
            //     bus::uart::baudrate_d baudrate = bus::uart::baudrate_d::BAUDRATE_115200,
            //     unsigned int databits = 8,
            //     bus::uart::stopbits_d stopbits = bus::uart::stopbits_d::ONE,
            //     bus::uart::parity_d parity = bus::uart::parity_d::NONE,
            //     bus::uart::flowcontrol_d flowcontrol = bus::uart::flowcontrol_d::NONE);

            sync_uart(const char* dev, unsigned int baud){
                this->_port = dev;
                console::info("port : {}", dev);
                switch (baud){
                    case 110: this->baudrate = bus::uart::baudrate_d::BAUDRATE_110; break;
                    case 300: this->baudrate = bus::uart::baudrate_d::BAUDRATE_300; break;
                    case 600: this->baudrate = bus::uart::baudrate_d::BAUDRATE_600; break;
                    case 1200: this->baudrate = bus::uart::baudrate_d::BAUDRATE_1200; break;
                    case 2400: this->baudrate = bus::uart::baudrate_d::BAUDRATE_2400; break;
                    case 4800: this->baudrate = bus::uart::baudrate_d::BAUDRATE_4800; break;
                    case 9600: this->baudrate = bus::uart::baudrate_d::BAUDRATE_9600; break;
                    case 14400: this->baudrate = bus::uart::baudrate_d::BAUDRATE_14400; break;
                    case 19200: this->baudrate = bus::uart::baudrate_d::BAUDRATE_19200; break;
                    case 38400: this->baudrate = bus::uart::baudrate_d::BAUDRATE_38400; break;
                    case 56000: this->baudrate = bus::uart::baudrate_d::BAUDRATE_56000; break;
                    case 57600: this->baudrate = bus::uart::baudrate_d::BAUDRATE_57600; break;
                    case 115200: this->baudrate = bus::uart::baudrate_d::BAUDRATE_115200; break;
                    case 128000: this->baudrate = bus::uart::baudrate_d::BAUDRATE_128000; break;
                    case 230400: this->baudrate = bus::uart::baudrate_d::BAUDRATE_230400; break;
                    case 256000: this->baudrate = bus::uart::baudrate_d::BAUDRATE_256000; break;
                    case 500000: this->baudrate = bus::uart::baudrate_d::BAUDRATE_500000; break;
                    case 750000: this->baudrate = bus::uart::baudrate_d::BAUDRATE_750000; break;
                    default:
                        this->baudrate = bus::uart::baudrate_d::BAUDRATE_9600;
                }
                this->databits = 8;
                this->stopbits = bus::uart::stopbits_d::ONE;
                this->parity = bus::uart::parity_d::NONE;
                this->flowcontrol = bus::uart::flowcontrol_d::NONE;
            }

            virtual ~sync_uart();

            /* sync bus interface function */
            bool is_open() override;
            bool open() override;
            void close() override;
            int read(uint8_t* data, int len) override;
            int read_until(uint8_t* data, int len, unsigned int t_ms) override;
            int write(const uint8_t* data, int len) override;

        private:
            #if defined(__linux__) || defined(__APPLE__)
                int _fd = -1;   //device file descriptor
            #endif
            string _port;
    }; //class sync_uart

    class async_uart : public oe::bus::uart, oe::bus::async_bus {
        public:
            async_uart() = default;
            virtual ~async_uart() = default;
    }; //class async_uart

} //namespace oe::bus

#endif