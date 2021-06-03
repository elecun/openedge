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
                this->close();
            }
            
            ~gpio() {
                this->close();
            }

            bool open() override {

                //export
                int fd_export = ::open("/sys/class/gpio/export", O_WRONLY);
                if(fd_export==-1)
                    return false;

                string p = fmt::format_int(this->get_pin()).str();
                if(::write(fd_export, p.c_str(), p.size())!=(int)p.size()){
                    return false;
                }
                ::close(fd_export);

                usleep(200000);

                //direction
                string access = fmt::format("/sys/class/gpio/gpio{}/direction", this->get_pin());

                int fd_dir = ::open(access.c_str(), O_WRONLY);
                if(fd_dir==-1){
                    return false;
                }
                
                string dir = this->get_dir();
                if(::write(fd_dir, dir.c_str(), dir.size())!=(int)dir.size()){
                    return false;
                }

                if(fd_dir)
                    ::close(fd_dir);

                return true;
            }

            bool close() override {

                //export
                int fd_unexport = -1;
                if((fd_unexport = ::open("/sys/class/gpio/unexport", O_WRONLY))==-1){
                    return false;
                }

                string p = fmt::format_int(this->get_pin()).str();
                if(::write(fd_unexport, p.c_str(), p.size())!=(int)p.size()){
                    ::close(fd_unexport);
                    return false;
                }
                
                if(fd_unexport)
                    ::close(fd_unexport);

                return true;

            }

            const LEVEL read() override {
                
                string access = fmt::format("/sys/class/gpio/gpio{}/value", this->get_pin());

                //value
                int fd_value = -1;
                if((fd_value = ::open(access.c_str(), O_RDONLY))==-1){
                    return LEVEL::ERROR;
                }

                char buffer[8] = {'\0', };
                if(::read(fd_value, &buffer[0], sizeof(buffer))>0){
                    ::close(fd_value);
                    int value = std::stoi(buffer);
                    if(value==1) return LEVEL::HIGH;
                    else return LEVEL::LOW;
                }

                if(fd_value)
                    ::close(fd_value);

                return LEVEL::ERROR;
            }

            void write(LEVEL value) override {
                console::info("Not implemented yet");
            }
    };

} /* namespace osd3358rt */
} /* namespace oe::support */

#endif