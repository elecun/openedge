
/**
 * @file    device.hpp
 * @brief   Deviuce Interface Class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_DEVICE_HPP_
#define _OPENEDGE_CORE_DEVICE_HPP_

#include <map>
#include <string>

using namespace std;

namespace oe::core {

    class iDevice {
        public:
            enum class DEVICE_STATUS : int { INACTIVE=0, ACTIVE };

            virtual DEVICE_STATUS getStatus() { return status; }
			virtual DEVICE_STATUS activate() { this->setStatus(DEVICE_STATUS::ACTIVE); return getStatus(); }
			virtual DEVICE_STATUS deactivate() { this->setStatus(DEVICE_STATUS::INACTIVE); return getStatus();}

            virtual bool open() = 0;    //open device
            virtual void close() = 0;   //close device

        protected:
            void setStatus(DEVICE_STATUS s){ status = s; }

        protected:
            DEVICE_STATUS status;
    };

    //class for Extendable Device
    template<typename _mod_index, typename _mtype>
    class iDeviceExtend : public iDevice {
        public:
            virtual bool open() override { iDevice::open(); }
            virtual void close() override { iDevice::close(); }

            virtual bool addModule(_mtype* module) = 0;
            virtual bool removeModule(_mod_index) = 0;
            virtual _mtype* getModule(_mod_index) = 0;

        protected:
            std::map<_mod_index, _mtype> slot;
    };

} //namespace oe::core

#endif