
#include "ina3221.hpp"
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>

//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h> //open
#include <unistd.h> //close

namespace oe {

    INA3221::INA3221(const char* dev, unsigned char address):_dev(dev), _address(address){
        
    }

    INA3221::~INA3221(){
        if(_devfd)
            ::close(_devfd);
    }

    bool INA3221::open(){
        if(_devfd<0){
            _devfd = ::open(_dev.c_str(), O_RDWR);
            if(_devfd>0){
                if(ioctl(_devfd, I2C_SLAVE, _address)<0){
                    ::close(_devfd);
                    return false;
                }        
            }
        }
        return true;
    }

    void INA3221::close(){
        if(_devfd){
            ::close(_devfd);
        }
    }

    unsigned short read(unsigned char register)
}