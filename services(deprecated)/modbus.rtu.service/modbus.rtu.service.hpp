
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
        vector<uint16_t> read_holding_registers(const uint16_t address, int size);
        bool write_holding_registers(const uint16_t address, vector<uint16_t> data);
        bool write_holding_register(const uint16_t address, uint16_t data);

        

    private:
        string _port {"/dev/tty0"};
        unsigned int _baudrate {9600};
        unsigned char _slave_id { 0 };

}; //class


EXPORT_SERVICE_API

#endif