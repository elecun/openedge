

#include "uart.hpp"
#include <openedge/log.hpp>

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

namespace oe::bus {

    syncUART::syncUART(const char* dev, busUART::busBaudrate baudrate, unsigned int databits, busUART::busStopbits stopbits,
            busUART::busParity parity, busUART::busFlowControl flowcontrol)
            :_port(dev), busUART(baudrate, databits, stopbits, parity, flowcontrol){

    }

    syncUART::~syncUART(){
        this->close();
    }

    bool syncUART::open(){

        #if defined(_WIN32) || defined(_WIN64)
            console::info("Not supported yet for Windows");
            return false;
        #elif defined(__linux__) || defined(__APPLE__)
            struct termios options;
            _fd = ::open(_port.c_str(), O_RDWR|O_NOCTTY|O_NDELAY);
            if(_fd!=-1){
                fcntl(_fd, F_SETFL, FNDELAY);   //open the device in nonblocking mode
                tcgetattr(_fd, &options);   //get the current options of the port
                bzero(&options, sizeof(options));

                speed_t baud;
                switch(baudrate){
                    case busUART::busBaudrate::BAUDRATE_110: baud = B110; break;
                    case busUART::busBaudrate::BAUDRATE_300: baud = B300; break;
                    case busUART::busBaudrate::BAUDRATE_600: baud = B600; break;
                    case busUART::busBaudrate::BAUDRATE_1200: baud = B1200; break;
                    case busUART::busBaudrate::BAUDRATE_2400: baud = B2400; break;
                    case busUART::busBaudrate::BAUDRATE_4800: baud = B4800; break;
                    case busUART::busBaudrate::BAUDRATE_9600: baud = B9600; break;
                    case busUART::busBaudrate::BAUDRATE_19200: baud = B19200; break;
                    case busUART::busBaudrate::BAUDRATE_38400: baud = B38400; break;
                    case busUART::busBaudrate::BAUDRATE_57600: baud = B57600; break;
                    case busUART::busBaudrate::BAUDRATE_115200: baud = B115200; break;
                    default: {
                        console::error("Invalid UART Baudrate");
                        return false;
                    }
                }

                int databits_flag = 0;
                switch(databits){
                    case 5: databits_flag = CS5; break;
                    case 6: databits_flag = CS6; break;
                    case 7: databits_flag = CS7; break;
                    case 8: databits_flag = CS8; break;
                    default: {
                        console::error("Invalid UART Databits");
                        return false;
                    }
                }

                int stopbits_flag = 0;
                switch(stopbits){
                    case busUART::busStopbits::ONE: stopbits_flag = 0; break;
                    case busUART::busStopbits::TWO: stopbits_flag = CSTOPB; break;
                    default: {
                        console::error("Invalid UART Stopbits");
                        return false;
                    }
                }

                int parity_flag = 0;
                switch(parity){
                    case busUART::busParity::NONE: parity_flag = 0; break;
                    case busUART::busParity::EVEN: parity_flag = PARENB; break;
                    case busUART::busParity::ODD: parity_flag = (PARENB|PARODD); break;
                    default: {
                        console::error("Invalid UART Parity");
                        return false;
                    }
                }

                cfsetispeed(&options, baud);    //set the baudrate
                cfsetospeed(&options, baud);
                options.c_cflag |= ( CLOCAL | CREAD | databits_flag | parity_flag | stopbits_flag);
                options.c_iflag |= ( IGNPAR | IGNBRK );
                options.c_cc[VTIME]=0;  //timer unused
                options.c_cc[VMIN]=0;   //at least on character before satisfy reading
                tcsetattr(_fd, TCSANOW, &options);  //activate the settings
            }

        #endif

        return true;
    }

    void syncUART::close(){
        #if defined(_WIN32) || defined (_WIN64)
            console::error("Not support on this OS yet.")
        #elif defined (__linux__) || defined (__APPLE__)
            close(_fd);
            _fd = -1;
        #endif
    }

    int syncUART::read(uint8_t* data, int len){

        #if defined(_WIN32) || defined (_WIN64)
            console::error("Not support on this OS yet.")
        #elif defined (__linux__) || defined (__APPLE__)
            
        #endif

        return 0;
    }

    int syncUART::write(const uint8_t* data, int len){

        #if defined(_WIN32) || defined (_WIN64)
            console::error("Not support on this OS yet.")
        #elif defined (__linux__) || defined (__APPLE__)
                return (int)::write(_fd, data, (size_t)len);
        #endif

        return 0;
    }


} /* namespace oe::bus */