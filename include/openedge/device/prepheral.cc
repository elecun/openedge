
#include "prepheral.hpp"
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <openedge/device/general.hpp>

namespace oe {

    prepheral::prepheral(device* dev, const char* pname):_pname(pname){
        this->open(dev->)
    }

    prepheral::~prepheral() {

    }

    bool prepheral::open(unsigned char addr){
        if(ioctl(_fd, I2C_SLAVE, addr)<0){
            ::close(_fd);
            return false;
        } 
        return true;
    }

    unsigned short prepheral::read(unsigned char address){
        
        return 0x0000;
    }

}