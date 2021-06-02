/**
 * @file    gpio.hpp
 * @brief   general purpose input/output
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.06.02
 **/

#ifndef _OPENEDGE_DEVICE_GPIO_HPP_
#define _OPENEDGE_DEVICE_GPIO_HPP_

#include <string>
#include <algorithm>

using namespace std;
 
namespace oe::device {
    
    class gpio {

        public:
            enum class LEVEL : int {
                ERROR = 0,
                LOW = 1,
                HIGH = 2
            };

            gpio(int pin_number, const char* direction):pin(pin_number), dir(direction){
                transform(dir.begin(), dir.end(), dir.begin(), ::tolower);
            }
            virtual ~gpio() = default;
                
            virtual bool open() = 0;
            virtual bool close() = 0;
            virtual const LEVEL read() = 0;
            virtual void write(LEVEL value) = 0;

            const int get_pin() const { return this->pin; }
            const char* get_dir() const { return this->dir.c_str(); }
            const bool is_out() {
                if(dir.compare("out")==0)
                    return true;
                return false;
            }

        protected:
            int pin = 1;
            string dir = "out";

    };

} /* namespace oe::device */

#endif