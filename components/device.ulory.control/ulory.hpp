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
#include <openedge/log.hpp>
#include <openedge/common/uart.hpp>

using namespace oe;
using namespace std;

namespace oe::device {
    namespace systembase {    

        class ulory {
            public:
                ulory(const char* dev_name, int baudrate, int timeout_s = 1)
                :_device_name(dev_name), _baudrate(baudrate), _timeout_s(timeout_s){
                    _uart = new oe::bus::sync_uart(dev_name, baudrate);
                }
                virtual ~ulory() {
                    if(_uart){
                        delete _uart;
                    }
                }

            
                bool is_open(){
                    return _uart->is_open();
                }

                void open(){
                    if(_uart->open()){
                        _uart->set_timeout(_timeout_s);
                    }
                }

                void close(){
                    if(_uart->is_open()){
                        _uart->close();
                    }
                }

                int read(char* buffer, int buffer_len){
                    uint8_t* data = new uint8_t[1024*8];
                    int rcv_len = _uart->read(data, sizeof(uint8_t)*1024*8);

                    if(rcv_len>0){
                        console::info("read bytes : {}", rcv_len);
                        string concat_data;
                        for(int i=0; i<rcv_len; i++){
                            concat_data += fmt::format("{0:x}\t", data[i]);
                        }
                        console::info("read : {}", concat_data);
                    }

                    if(buffer_len<rcv_len){
                         console::warn("Buffer size is less than read bytes");
                    }
                    delete []data;

                    return rcv_len;
                }

            private:
                oe::bus::sync_bus* _uart = nullptr;

                string _device_name;
                int _baudrate;
                int _timeout_s = 1; //1 sec
                
            
        }; /* class */

    } /* namespace waterlink */
} /* namespace device */

#endif