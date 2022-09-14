/**
 * @file uloray.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief sysbase ulory device
 * @version 0.1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_SUPPORT_DEVICE_SYSTEMBASE_ULORY_HPP_
#define _OPENEDGE_SUPPORT_DEVICE_SYSTEMBASE_ULORY_HPP_

#include <vector>
#include <array>
#include <algorithm>
#include <string>
#include <sstream>
//#include <openedge/common/bus.hpp> //for sync_bus
#include <openedge/log.hpp>

//with boost library
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace oe;
using namespace std;

namespace oe::device {
    namespace systembase {    

        class ulory {
            public:
                //ulory(bus::sync_bus* bus):_bus(bus){ }
                ulory(const char* dev_name, unsigned int baudrate):_io(), _port(_io, dev_name){
                    _port.set_option( boost::asio::serial_port_base::parity());	// default none
                    _port.set_option( boost::asio::serial_port_base::character_size(8));
                    _port.set_option( boost::asio::serial_port_base::stop_bits());	// default one
                    _port.set_option( boost::asio::serial_port_base::baud_rate(baudrate));

                    read_some();
                    boost::thread t(boost::bind(&boost::asio::io_service::run, &_io));
                }
                virtual ~ulory() = default;

                typedef struct _version {
                    int _major, _minor, _patch;
                    string str(){
                        return fmt::format("{}.{}.{}", _major, _minor, _patch);
                    }
                    _version(int mj, int mi, int pt):_major(mj),_minor(mi),_patch(pt){}
                } version;

            
                bool is_open(){
                    return _port.is_open();
                }

                void close(){
                    _port.close();
                }

                int read_async(){
                    this->read_some();
                }
                

                /**
                 * @brief protocol version response
                 * @return version
                 */
                version get_version(){
                    if(_port.is_open()){
                        const unsigned char packet[] = {'w', 'c', 'v', '*'};
                        _port.write_some(boost::asio::buffer(packet, sizeof(packet)));
                    }
                    else {
                        console::warn("Waterlinked M64 interface is not working...");
                    }
                    return version(0,0,1);
                }

                // string help(){
                //     const char packet[] = {'a', 't', '&', 'v', 0x0d}; //with carriage return
                //     _port.write_some(boost::asio::buffer(packet, sizeof(packet)));
                //     bool rc = transaction(command, 5000);
                // }

                // string get_parameters(){
                //     const char* command = "at&v";
                //     int byte = transfer(command);
                //     if(byte!=(int)strlen(command))
                //         return string("");
                // }

            private:
                /**
                 * @brief transfer the dedicated AT command for uLory
                 * 
                 * @param command string AT command
                 * @return int transferred bytes
                 */
                // int transfer(const char* command){
                //     if(_bus){
                //         if(_bus->is_open()){
                //             const unsigned char* ucommand = reinterpret_cast<const unsigned char*>(command);
                //             return _bus->write(ucommand, (int)strlen((const char*)command));
                //         }
                //         else {
                //             console::warn("Bus interface is closed.");
                //         }
                //     }
                //     else {
                //         console::warn("Invalid bus interface. It cannot be transferred.");
                //     }
                //     return 0;
                // }

                // bool transaction(const char* command, unsigned int timeout, unsigned int space = 100){
                //     if(_bus){
                //         if(_bus->is_open()){

                //             /* write command */
                //             const unsigned char* ucommand = reinterpret_cast<const unsigned char*>(command);
                //             int wbyte = _bus->write(ucommand, (int)strlen((const char*)command));
                //             if(wbyte!=(int)strlen(command)){
                //                 console::warn("Wrong transfer : {}bytes transferred", wbyte);
                //                 return false;
                //             }

                //             /* receive data until  */
                //             #define MAX_RCV_BUFFER  2048
                //             unsigned char* rcvdata = new unsigned char[MAX_RCV_BUFFER];
                //             int rbyte = _bus->read_until(rcvdata, sizeof(unsigned char)*MAX_RCV_BUFFER, timeout, space);
                //             if(rbyte>0)
                //                 console::info("{}bytes received", rbyte);
                            
                //             // const char* data = reinterpret_cast<const char*>(rcvdata);
                //             // std::cout << data << endl;
                //             // string strdata {data};
                //             // console::info("received : {}", strdata);
                //             delete []rcvdata;

                //             return true;
                //         }
                //         else {
                //             console::warn("Bus interface is closed.");
                //         }
                //     }
                //     else {
                //         console::warn("Invalid bus interface. It cannot be transferred.");
                //     }
                //     return false;
                // }

            private:
                void handler(const boost::system::error_code& error, size_t bytes_transferred){
                    _read_data[bytes_transferred]=0;
                    read_some();
                }

                void read_some(){
                    _port.async_read_some(boost::asio::buffer(_read_data,1024), 
                        boost::bind(&ulory::handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
                }

            private:
                char _read_data[1024] = { 0x00, };
                boost::asio::io_service _io;
                boost::asio::serial_port _port;
            
        }; /* class */

    } /* namespace waterlink */
} /* namespace device */

#endif