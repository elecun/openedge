/**
 * @file    dkm_dx3000.hpp
 * @brief   DKM DX3000 AC Motor Driver with ModbusRTU (RS485)
 * @author  Byunghun Hwang<bh.hwnag@iae.re.kr>
 * @version 0.0.1
 * @note    
 */


#ifndef _OPENEDGE_SUPPORT_DEVICE_DKM_DX3000_HPP_
#define _OPENEDGE_SUPPORT_DEVICE_DKM_DX3000_HPP_

#include <openedge/device/controller.hpp>
#include <variant>
#include <string>


using namespace std;

namespace oe::device { class bus; }
namespace oe::support {

class DKM_DX3000 : public oe::device::controller {

    public:
        enum class BAUDRATE : int { 
            BAUDRATE_2400 = 0,
            BAUDRATE_9600 = 1,
            BAUDRATE_19200 = 2,
            BAUDRATE_38400 = 3,
            BAUDRATE_115200 = 4
        };

        //DX3000 driver supports setting configuration
        enum class DX3000_SET : int {
            SET_DEFAULT = 0,    //manually set default configuration
            SET_MAX_SPEED = 1,
            SET_SPEED_LIMIT,
            SET_ACC_TIME,
            SET_DIR,
            SET_GEAR_RATIO,
            SET_CONTROL_MODE,
            SET_ZERO_CLAMP,
            SET_RPM_OFFSET,
            SET_GAIN_P,
            SET_GAIN_I,
            SET_ID,
            SET_IO_CONFIG,
            SET_SPEED_CONFIG,
            SET_BAUDRATE,
            SET_MEM_RPM1,
            SET_MEM_RPM2,
            SET_MEM_RPM3,
            SET_MEM_TORQUE1,
            SET_MEM_TORQUE2,
            SET_MEM_TORQUE3
        };

        DKM_DX3000(oe::device::bus* bus = nullptr);
        DKM_DX3000(const char* dev = nullptr, BAUDRATE baudrate = BAUDRATE::BAUDRATE_9600);
        virtual ~DKM_DX3000();

        //device interface
        bool open() override;
        void close() override;

        /* interface APIs*/
        bool init();    //motor driver initialization
        bool move();    //move motor
        void stop();    //stop motor
        bool set_parameter(DX3000_SET opt, variant<int, double> param);

    private:
        oe::device::bus* _bus = nullptr;
        string _dev;
        unsigned int _baudrate = 0;

};

} //namespace


#endif