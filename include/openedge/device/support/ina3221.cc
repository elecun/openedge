
#include "ina3221.hpp"
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>

//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h> //open
#include <unistd.h> //close

#include <openedge/device/general.hpp>

/**
 * pre-requisites
 *  - $ sudo apt-get install libi2c-dev
 */

namespace oe::support {

    INA3221::INA3221(oe::device* device, unsigned char address){

    }

    INA3221::~INA3221(){

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

    unsigned short INA3221::read(unsigned char address){
        unsigned short buf = 0x0000;
        if(::read(_devfd, &buf, sizeof(unsigned short))!=sizeof(unsigned short)){
            return 0x0000;
        }
        return buf;
    }
}