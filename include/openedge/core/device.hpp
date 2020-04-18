
/**
 * @file    device.hpp
 * @brief   Deviuce Interface Class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_DEVICE_HPP_
#define _OPENEDGE_CORE_DEVICE_HPP_

namespace oe::core {

    class idevice {
        public:
            enum class DEVICE_STATUS : int { INACTIVE=0, ACTIVE };

            virtual DEVICE_STATUS getStatus() { return status; }
			virtual DEVICE_STATUS activate() { this->setStatus(DEVICE_STATUS::ACTIVE); return getStatus(); }
			virtual DEVICE_STATUS deactivate() { this->setStatus(DEVICE_STATUS::INACTIVE); return getStatus();}

        protected:
            void setStatus(DEVICE_STATUS s){ status = s; }

        protected:
            DEVICE_STATUS status;
    };

} //namespace oe::core

#endif