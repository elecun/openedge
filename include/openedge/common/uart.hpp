
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

using namespace std;

namespace oe::bus {

    class syncUART : public oe::interface::busUART  {
    public:
        syncUART(const char* dev, busUART::busBaudrate baudrate = busUART::busBaudrate::BAUDRATE_115200,
            unsigned int databits = 8,
            busUART::busStopbits stopbits = busUART::busStopbits::ONE,
            busUART::busParity parity = busUART::busParity::NONE,
            busUART::busFlowControl flowcontrol = busUART::busFlowControl::NONE);
        virtual ~syncUART();

        /* bus interface function */
        bool open() override;
        void close() override;
        int read(uint8_t* data, int len) override;
        int write(const uint8_t* data, int len) override;

    private:
        #if defined(__linux__) || defined(__APPLE__)
            int _fd = -1;   //device file descriptor
        #endif
        string _port;
        
    }; //class

} //namespace oe::bus

#endif