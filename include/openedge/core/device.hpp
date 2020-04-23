
/**
 * @file    device.hpp
 * @brief   Deviuce Interface Class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_DEVICE_HPP_
#define _OPENEDGE_CORE_DEVICE_HPP_

#include <map>
#include <string>
#include <stdint.h>

using namespace std;

namespace oe::core {

    class iDevice {
        public:
            enum class DEVICE_STATUS : int { INACTIVE=0, ACTIVE };
            enum class DEVICE_TYPE : int { 
                UNKNOWN = 0, 
                BUS,
            };

            virtual DEVICE_STATUS getStatus() { return status; }
			virtual DEVICE_STATUS activate() { this->setStatus(DEVICE_STATUS::ACTIVE); return getStatus(); }
			virtual DEVICE_STATUS deactivate() { this->setStatus(DEVICE_STATUS::INACTIVE); return getStatus();}

            virtual bool open() = 0;    //open device
            virtual void close() = 0;   //close device

        protected:
            void setStatus(DEVICE_STATUS s){ status = s; }

        protected:
            DEVICE_STATUS status = DEVICE_STATUS::INACTIVE;
            DEVICE_TYPE type = DEVICE_TYPE::UNKNOWN;
    };

    //class for Extendable Device
    template<typename _indexType, typename _moduleType>
    class iDeviceExtendable : public iDevice {
        public:
            virtual bool addModule(_moduleType* module) = 0;
            virtual bool removeModule(_indexType) = 0;
            virtual _moduleType* getModule(_indexType) = 0;

        protected:
            std::map<_indexType, _moduleType> slot;
    };

    //general PLC Device interface
    namespace bus{ class iDeviceBus; }
    class iDevicePLC : public iDevice {
        public:
            virtual uint8_t readByte(const char* address) = 0;
            virtual bool configBus(bus::iDeviceBus* bus) = 0;
    };

} //namespace oe::core

#endif