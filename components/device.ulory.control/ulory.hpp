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
                ulory(const char* dev_name, int baudrate, int timeout_ms = 0)
                :_device_name(dev_name), _baudrate(baudrate), _timeout_ms(timeout_ms){
                    
                }
                virtual ~ulory() = default;

            
                bool is_open(){
                    return false;
                }

                void open(){

                }

                void close(){
                    
                }

                int async_read(char* buffer, int size){
                    
                }

            private:
                oe::bus::uart* _uart = nullptr;

                string _device_name;
                int _baudrate;
                int _timeout_ms;
                
            
        }; /* class */

    } /* namespace waterlink */
} /* namespace device */

#endif