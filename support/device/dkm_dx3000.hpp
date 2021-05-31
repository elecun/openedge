/**
 * @file    dkm_dx3000.hpp
 * @brief   DKM DX3000 AC Motor Driver with ModbusRTU (RS485)
 * @author  Byunghun Hwang<bh.hwnag@iae.re.kr>
 * @version 0.0.1
 * @note    
 */


#ifndef _OPENEDGE_SUPPORT_DEVICE_DKM_DX3000_HPP_
#define _OPENEDGE_SUPPORT_DEVICE_DKM_DX3000_HPP_

#include <openedge/device.hpp>

namespace oe::support {

class DKM_DX3000 : public oe::device {

    public:
        enum class BAUDRATE : int { 
            BAUDRATE_2400 = 0,
            BAUDRATE_9600 = 1,
            BAUDRATE_19200 = 2,
            BAUDRATE_38400 = 3,
            BAUDRATE_115200 = 4
        };

        DKM_DX3000(oe::bus* bus = nullptr);
        virtual ~DKM_DX3000();

        /* interface APIs*/
        bool init();    //motor driver initialization
        bool move();    //move motor
        void stop();    //stop motor

        //parameter settings
        void set_speed_max(const int rpm = 1760);
        void set_speed_limit(const int rpm = 1760);
        void set_acc_time(const double sec = 0.1);
        void set_dir(const int cw=0);
        void set_gear_ratio(const double ratio=1.0);
        void set_control_mode(const int mode=1); //1=rpm, 0=torque
        void set_zero_clamp(const double voltage=0.0);
        void set_rpm_offset(const double voltage=0.0); 
        void set_gain_p(const int gain=100); //0~255
        void set_gain_i(const int gain=50); //0~255
        void set_id(const int id=1);
        void set_io_strategy(const int strategy=1); //0-CN3, 1=RS485
        void set_speed_strategy(const int strategy=1); //0=CN4, 1=RS485
        void set_baudrate(const BAUDRATE = BAUDRATE::BAUDRATE_9600);
        void set_mem_rpm1(const int rpm=500);
        void set_mem_rpm2(const int rpm=1000);
        void set_mem_rpm3(const int rpm=1500);
        void set_mem_torque1(const int percentage=10);
        void set_mem_torque2(const int percentage=20);
        void set_mem_torque3(const int percentage=50);

    private:


};

} //namespace


#endif