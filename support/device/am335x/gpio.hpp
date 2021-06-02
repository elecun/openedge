/**
 * @file    gpio.hpp
 * @brief   general purpose input/output
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.06.02
 **/

#ifndef _OPENEDGE_SUPPORT_DEVICE_AM335XRT_GPIO_HPP_
#define _OPENEDGE_SUPPORT_DEVICE_AM335XRT_GPIO_HPP_

#include <openedge/device/gpio.hpp>
#include <openedge/log.hpp>
#include <string>

using namespace std;

namespace oe::support {

namespace am335xrt {
    class gpio : public oe::device::gpio {
        public:
            gpio(int pin, const char* dir):oe::device::gpio(pin, dir) {

            }
            
            ~gpio() {
                close();
            }

            bool open() override {

                string pin = fmt::format("{}", this->get_pin());

                //export
                int fd_export = ::open("/sys/class/gpio/export", O_WRONLY);
                if(fd_export==-1){
                    console::error("Unable to open gpio{} export file", this->get_pin());
                    return false;
                }

                if(::write(fd_export, pin.c_str(), pin.size())!=(int)pin.size()){
                    return false;
                }
                ::close(fd_export);

                //direction
                int fd_dir = ::open("/sys/class/gpio/direction", O_WRONLY);
                if(fd_dir==-1){
                    console::error("Unable to open gpio{} direction file", this->get_pin());
                    return false;
                }

                if(::write(fd_dir, pin.c_str(), pin.size())!=(int)pin.size()){
                    return false;
                }
                ::close(fd_dir);


                return true;
            }

            bool close() override {

                string pin = fmt::format("{}", this->get_pin());

                //export
                int fd_unexport = ::open("/sys/class/gpio/unexport", O_WRONLY);
                if(fd_unexport==-1){
                    console::error("Unable to open gpio{} unexport file", this->get_pin());
                    return false;
                }

                if(::write(fd_unexport, pin.c_str(), pin.size())!=(int)pin.size()){
                    return false;
                }
                ::close(fd_unexport);

            }

            const LEVEL read() override {
                
                string access = fmt::format("/sys/class/gpio/gpio{}", this->get_pin());

                //value
                int fd_value = ::open(access.c_str(), O_WRONLY);
                if(fd_value==-1){
                    console::error("Unable to open gpio{} value file", this->get_pin());
                    return LEVEL::ERROR;
                }
                int value = -1;
                if(::read(fd_value, &value, sizeof(value))){
                    if(value==1) return LEVEL::HIGH;
                    else return LEVEL::LOW;
                }
                ::close(fd_value);
            }

            void write(LEVEL value) override {

            }
    };

} /* namespace osd3358rt */
} /* namespace oe::support */

#endif