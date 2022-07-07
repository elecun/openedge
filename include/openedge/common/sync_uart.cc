

#include "uart.hpp"
#include <openedge/log.hpp>
#include <sys/stat.h>
#include <fcntl.h>


#define MAX_RCV_BUFFER_SIZE 4096    //Maximum size of receive buffer

namespace oe::bus {

    // sync_uart::sync_uart(const char* dev, uart::baudrate_d baudrate, unsigned int databits, uart::stopbits_d stopbits,
    //         uart::parity_d parity, uart::flowcontrol_d flowcontrol)
    //         :_port(dev), bus::uart(baudrate, databits, stopbits, parity, flowcontrol){

    // }

    sync_uart::~sync_uart(){
        this->close();
    }

    bool sync_uart::is_open(){
        if(_fd!=-1)
            return true;
        return false;
    }

    bool sync_uart::open(){

        #if defined(_WIN32) || defined(_WIN64)
            console::info("Not supported yet for Windows");
            return false;
        #elif defined(__linux__) || defined(__APPLE__)
            struct termios options;
            console::info("sync_uart open : {}", _port);
            _fd = ::open(_port.c_str(), O_RDWR|O_NOCTTY|O_NDELAY);
            if(_fd!=-1){
                fcntl(_fd, F_SETFL, FNDELAY);   //open the device in nonblocking mode
                tcgetattr(_fd, &options);   //get the current options of the port
                bzero(&options, sizeof(options));

                speed_t baud;
                switch(baudrate){
                    case uart::baudrate_d::BAUDRATE_110: baud = B110; break;
                    case uart::baudrate_d::BAUDRATE_300: baud = B300; break;
                    case uart::baudrate_d::BAUDRATE_600: baud = B600; break;
                    case uart::baudrate_d::BAUDRATE_1200: baud = B1200; break;
                    case uart::baudrate_d::BAUDRATE_2400: baud = B2400; break;
                    case uart::baudrate_d::BAUDRATE_4800: baud = B4800; break;
                    case uart::baudrate_d::BAUDRATE_9600: baud = B9600; break;
                    case uart::baudrate_d::BAUDRATE_19200: baud = B19200; break;
                    case uart::baudrate_d::BAUDRATE_38400: baud = B38400; break;
                    case uart::baudrate_d::BAUDRATE_57600: baud = B57600; break;
                    case uart::baudrate_d::BAUDRATE_115200: baud = B115200; break;
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
                    case uart::stopbits_d::ONE: stopbits_flag = 0; break;
                    case uart::stopbits_d::TWO: stopbits_flag = CSTOPB; break;
                    default: {
                        console::error("Invalid UART Stopbits");
                        return false;
                    }
                }

                int parity_flag = 0;
                switch(parity){
                    case uart::parity_d::NONE: parity_flag = 0; break;
                    case uart::parity_d::EVEN: parity_flag = PARENB; break;
                    case uart::parity_d::ODD: parity_flag = (PARENB|PARODD); break;
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
            else {
                console::error("{} open failed.(error : {})", _port, _fd);
            }

        #endif

        return true;
    }

    void sync_uart::close(){
        #if defined(_WIN32) || defined (_WIN64)
            console::error("Not support on this OS yet.")
        #elif defined (__linux__) || defined (__APPLE__)
            ::close(_fd);
            _fd = -1;
        #endif
    }

    int sync_uart::read(uint8_t* data, int len){

        #if defined(_WIN32) || defined (_WIN64)
            console::error("Not support on this OS yet.")
        #elif defined (__linux__) || defined (__APPLE__)
            return ::read(_fd, data, len);
        #endif

        return 0;
    }

    int sync_uart::read_until(uint8_t* data, int len, unsigned int t_ms){
        #if defined(_WIN32) || defined (_WIN64)
            console::error("Not support on this OS yet.")
        #elif defined (__linux__) || defined (__APPLE__)
            timeout _timer;
            uint8_t* rcv_buffer = new uint8_t[MAX_RCV_BUFFER_SIZE];
            int rcv_counter = 0;
            while(_timer.elapsed_ms()<t_ms || t_ms==0){
                uint8_t* rcv = new uint8_t[MAX_RCV_BUFFER_SIZE];
                ssize_t size = ::read(_fd, rcv, MAX_RCV_BUFFER_SIZE);
                if(size>0){
                    memcpy(rcv_buffer+sizeof(uint8_t)*rcv_counter, rcv, size);
                    rcv_counter += size;
                }
                delete []rcv;

                if((int)rcv_counter==0)
                    return -1;  //no received data
            }
        #endif

        return 0;
    }

    int sync_uart::write(const uint8_t* data, int len){

        #if defined(_WIN32) || defined (_WIN64)
            console::error("Not support on this OS yet.")
        #elif defined (__linux__) || defined (__APPLE__)
            console::info("write to bus");
            return (int)::write(_fd, data, (size_t)len);
        #endif

        return 0;
    }


} /* namespace oe::bus */