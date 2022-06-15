
/**
 * @file    uart.hpp
 * @brief   UART Bus Concrete Class *sync, async)
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_BUS_UART_HPP_
#define _OPENEDGE_BUS_UART_HPP_

#include <openedge/common/bus.hpp>
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

    class sync_uart : public oe::interface::bus_uart, public oe::interface::sync_bus {
        public:
            sync_uart(const char* dev, bus_uart::baudrate_d baudrate = bus_uart::baudrate_d::BAUDRATE_115200,
                unsigned int databits = 8,
                bus_uart::stopbits_d stopbits = bus_uart::stopbits_d::ONE,
                bus_uart::parity_d parity = bus_uart::parity_d::NONE,
                bus_uart::flowcontrol_d flowcontrol = bus_uart::flowcontrol_d::NONE){}
            virtual ~sync_uart();

            /* sync bus interface function */
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

    class async_uart : public oe::interface::bus_uart, oe::interface::async_bus {
        public:
            async_uart() = default;
            virtual ~async_uart() = default;
    }; //class async_uart

} //namespace oe::bus

#endif