/**
 * @file    busi2c.hpp
 * @brief   I2C interface bus inteface
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.01.27
 * @note    Single device has multiple addresses
 **/

#ifndef _OPENEDGE_DEVICE_BUSI2C_HPP_
#define _OPENEDGE_DEVICE_BUSI2C_HPP_

#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <string>

using namespace std;

namespace oe {
    
    class busI2C {
        public:
            busI2C(const char* devicename): _devicename(devicename){
                _fd = ::open(devicename, O_RDWR); //open device file desc. only
            }
            virtual ~busI2C() {
                if(_fd)
                    ::close(_fd);
            }

            /**
             * @brief   device open
             * @param   devicename  device name (ex. /dev/i2c-2)
             **/
            bool open(const char* devicename, unsigned char addr){
                if(_fd<0){
                    _fd = ::open(devicename, O_RDWR);
                    if(_fd>0){
                        if(ioctl(_fd, I2C_SLAVE, addr)<0){
                            ::close(_fd);
                            return false;
                        }        
                    }
                }
                return true;
            }

            /**
             * @brief   close device if it is opened
             **/
            void close(){
                if(_fd){
                    ::close(_fd);
                }
            }

            /**
             * @brief   check if device is opened
             **/
            bool isOpen(){
                if(_fd<0)
                    return false;
                return true;
            }

            /**
             * @brief   check available for I2C Address
             **/
            bool isAvailable(unsigned char addr){
                if(_fd<0)
                    return false;
                return true;
            }
            
            /**
             * @brief   read 1 byte from address
             **/
            uint8_t read(uint8_t addr){
                if(this->isOpen() && this->isAvailable(addr)){
                    unsigned char buf = 0x00;
                    if(::read(_fd, &buf, sizeof(unsigned char))!=sizeof(unsigned char))
                        return 0x00;
                    return buf;
                }
            }

            uint8_t readRegister(uint8_t addr, uint8_t reg){
                
            }

            int writeByte(uint8_t data){

            }

            /**
             * @brief   getting device name (/dev/i2c-2)
             **/
            const char* getDeviceName() { return _devicename.c_str(); }

        private:
            int _fd = -1;
            string _devicename = "";
            unsigned char _address = 0x00;
    };

} /* namespace oe */

#endif