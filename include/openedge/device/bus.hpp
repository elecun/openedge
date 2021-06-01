/**
 * @file    bus.hpp
 * @brief   general bus interface
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.01.27
 **/

#ifndef _OPENEDGE_DEVICE_BUS_HPP_
#define _OPENEDGE_DEVICE_BUS_HPP_

#include <functional>
#include <string>

using namespace std;
 
namespace oe::device {

    typedef function<void(unsigned char*, int)> asyncCallback;
    
    class bus {

        public:
            bus(const char* dev):_dev(dev){}
            ~bus() = default;
            
            virtual bool open() = 0;
            virtual void close() = 0;
            virtual int read(unsigned char* data, int len) = 0;
            virtual int write(const unsigned char* data, int len) = 0;
            virtual void flush() = 0;

        protected:
            const char* get_devname() const { return _dev.c_str(); }

        protected:
            asyncCallback callback = nullptr;

        private:
            string _dev;

    };

} /* namespace oe::device */

#endif