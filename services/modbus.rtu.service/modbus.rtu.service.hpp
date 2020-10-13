
/**
 * @file    modbus.rtu.service.hpp
 * @brief   Modbus RTU Service
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_MODBUS_RTU_SERVICE_HPP_
#define _OPENEDGE_SERVICE_MODBUS_RTU_SERVICE_HPP_

#include <openedge/core/service.hpp>

using namespace oe;

class modbusRtuService : public core::iService {
    public:
        modbusRtuService();
        virtual ~modbusRtuService();
        
        //common iservice interface
        bool initService(const char* config = nullptr) override;
        bool closeService() override;

        //services APIs
        unsigned char read(const unsigned char address /*json*/);

    private:
        string _port {"/dev/tty0"};
        unsigned int _baudrate {9600};

}; //class


EXPORT_SERVICE_API

#endif