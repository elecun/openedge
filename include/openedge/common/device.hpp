
/**
 * @file    device.hpp
 * @brief   Deviuce Interface Class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_INTERFACE_DEVICE_HPP_
#define _OPENEDGE_INTERFACE_DEVICE_HPP_

#include <map>
#include <string>
#include <stdint.h>

using namespace std;

namespace oe::interface {

    /**
     * @brief Device Interface Class
     * 
     */
    class device {
        public:
            /**
             * @brief Type definition for Device
             * 
             */
            enum class dStatus : int { INACTIVE=0, ACTIVE };
            enum class dType : int { 
                UNKNOWN = 0, 
                BUS,
                SENSOR,
                CONTROLLER
            };

            /**
             * @brief Get the Status of the device
             * 
             * @return dStatus 
             */
            virtual const dStatus getStatus() const { return status; }
			virtual const dStatus activate() { this->setStatus(dStatus::ACTIVE); return getStatus(); }
			virtual const dStatus deactivate() { this->setStatus(dStatus::INACTIVE); return getStatus();}

            /**
             * @brief Device Open Interface Function
             * 
             * @return true if success
             * @return false 
             */
            virtual bool open() = 0;

            /**
             * @brief Device Close Interface Function
             * 
             */
            virtual void close() = 0;   //close device

        protected:
            /**
             * @brief set device status to ACTIVE or INACTIVE
             * 
             * @param s status (dStatus::INACTIVE or dSTtatus::ACTIVE)
             */
            void setStatus(dStatus s){ status = s; }

        protected:
            dStatus status = dStatus::INACTIVE;
            dType type = dType::UNKNOWN;
    }; /* device class */

} //namespace oe::core

#endif