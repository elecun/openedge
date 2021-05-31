
#include "dkm_dx3000.hpp"

namespace oe::support {


    DKM_DX3000::DKM_DX3000(oe::bus* bus = nullptr){

    }

    DKM_DX3000::~DKM_DX3000(){

    }

    bool DKM_DX3000::init(){
        
    }
    bool DKM_DX3000::move();    //move motor
    void DKM_DX3000::stop();    //stop motor

    //parameter settings
    void DKM_DX3000::set_speed_max(const int rpm = 1760);
    void DKM_DX3000::set_speed_limit(const int rpm = 1760);
    void DKM_DX3000::set_acc_time(const double sec = 0.1);
    void DKM_DX3000::set_dir(const int cw=0);
    void DKM_DX3000::set_gear_ratio(const double ratio=1.0);
    void DKM_DX3000::set_control_mode(const int mode=1); //1=rpm, 0=torque
    void DKM_DX3000::set_zero_clamp(const double voltage=0.0);
    void DKM_DX3000::set_rpm_offset(const double voltage=0.0); 
    void DKM_DX3000::set_gain_p(const int gain=100); //0~255
    void DKM_DX3000::set_gain_i(const int gain=50); //0~255
    void DKM_DX3000::set_id(const int id=1);
    void DKM_DX3000::set_io_strategy(const int strategy=1); //0-CN3, 1=RS485
    void DKM_DX3000::set_speed_strategy(const int strategy=1); //0=CN4, 1=RS485
    void DKM_DX3000::set_baudrate(const BAUDRATE = BAUDRATE::BAUDRATE_9600);
    void DKM_DX3000::set_mem_rpm1(const int rpm=500);
    void DKM_DX3000::set_mem_rpm2(const int rpm=1000);
    void DKM_DX3000::set_mem_rpm3(const int rpm=1500);
    void DKM_DX3000::set_mem_torque1(const int percentage=10);
    void DKM_DX3000::set_mem_torque2(const int percentage=20);
    void DKM_DX3000::set_mem_torque3(const int percentage=50);

} //namespace

