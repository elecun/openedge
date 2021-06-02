
#include "dkm_dx3000.hpp"
#include <openedge/log.hpp>
#include <openedge/device/bus.hpp>
#include <3rdparty/libmodbus/modbus.h>
#include <3rdparty/libmodbus/modbus-rtu.h>

static modbus_t* _modbus = nullptr;

namespace oe::support {


    DKM_DX3000::DKM_DX3000(oe::device::bus* bus){
    
    }
    DKM_DX3000::DKM_DX3000(const char* dev, BAUDRATE baudrate)
    :_dev(dev) {
        switch(baudrate){
            case BAUDRATE::BAUDRATE_2400: _baudrate = 2400; break;
            case BAUDRATE::BAUDRATE_9600: _baudrate = 9600; break;
            case BAUDRATE::BAUDRATE_19200: _baudrate = 19200; break;
            case BAUDRATE::BAUDRATE_38400: _baudrate = 38400; break;
            case BAUDRATE::BAUDRATE_115200: _baudrate = 115200; break;
            default:
                _baudrate = 9600;
        }
    }

    DKM_DX3000::~DKM_DX3000(){
        if(_modbus){
            modbus_close(_modbus);
            modbus_free(_modbus);
        }
    }

    bool DKM_DX3000::open(){

        if(!_modbus){
            _modbus = modbus_new_rtu(_dev.c_str(), _baudrate, 'N', 8, 1);
            if(!_modbus){
                console::warn("Unable to create the modbus context");
                return false;
            }

            modbus_set_slave(_modbus, 1);

            if(modbus_connect(_modbus)==-1){
                console::error("Unable to connect {}", modbus_strerror(errno));
                modbus_free(_modbus);
                return false;
            }
        }
    
        return true;
    }

    void DKM_DX3000::close(){
        if(_modbus){
            modbus_close(_modbus);
            modbus_free(_modbus);
        }
    }

    bool DKM_DX3000::init(){
        return true;

    }

    bool DKM_DX3000::move(){
        return true;
    }

    void DKM_DX3000::stop(){

    }

    bool DKM_DX3000::set_parameter(PARAMETER opt, variant<int, double> param){

        if(!_bus){
            return false;
        }

        switch(opt){
            case PARAMETER::SET_DEFAULT: {} break;
            case PARAMETER::SET_MAX_SPEED: {} break;
            case PARAMETER::SET_SPEED_LIMIT:{}  break;
            case PARAMETER::SET_ACC_TIME: {} break;
            case PARAMETER::SET_DIR: {} break;
            case PARAMETER::SET_GEAR_RATIO: {} break;
            case PARAMETER::SET_CONTROL_MODE: {} break;
            case PARAMETER::SET_ZERO_CLAMP: {} break;
            case PARAMETER::SET_RPM_OFFSET:{}  break;
            case PARAMETER::SET_GAIN_P: {} break;
            case PARAMETER::SET_GAIN_I: {} break;
            case PARAMETER::SET_ID: {} break;
            case PARAMETER::SET_IO_CONFIG: {} break;
            case PARAMETER::SET_SPEED_CONFIG: {} break;
            case PARAMETER::SET_BAUDRATE: {} break;
            case PARAMETER::SET_MEM_RPM1: {} break;
            case PARAMETER::SET_MEM_RPM2: {} break;
            case PARAMETER::SET_MEM_RPM3: {} break;
            case PARAMETER::SET_MEM_TORQUE1: {} break;
            case PARAMETER::SET_MEM_TORQUE2: {} break;
            case PARAMETER::SET_MEM_TORQUE3: {} break;
            default:
                console::warn("Setting configuration({}) does not exist", static_cast<int>(opt));
        }

        return true;
    }

} //namespace

