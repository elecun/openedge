
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
            enum class status_d : int { INACTIVE=0, ACTIVE };
            enum class type_d : int { 
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
            virtual const status_d get_status() const { return status; }
			virtual const status_d activate() { this->set_status(status_d::ACTIVE); return get_status(); }
			virtual const status_d deactivate() { this->set_status(status_d::INACTIVE); return get_status();}

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
            void set_status(status_d s) { status = s; }

        protected:
            status_d status = status_d::INACTIVE;
            type_d type = type_d::UNKNOWN;
    }; /* device class */

} //namespace oe::core

#endif