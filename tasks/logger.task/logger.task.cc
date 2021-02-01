

#include "logger.task.hpp"
#include <openedge/device/prepheral.hpp>
#include <openedge/device/general.hpp>
#include <fstream>

using namespace std;

//static component instance that has only single instance
static loggerTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new loggerTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}



bool loggerTask::configure(){

    //getting device information
    json _prepheral = json::parse(getProfile()->get("prepheral"));
    json _sensor1 = _prepheral["sensor-1"];
    json _sensor2 = _prepheral["sensor-2"];

    _device = new oe::device("/dev/i2c-2"); //I2C Device Open
    if(_device->open()){
        _device->addPrepheral(new oe::prepheral(_device, "sensor-1"));
        _device->addPrepheral(new oe::prepheral(_device, "sensor-2"));
    }
    else {
        spdlog::error("device cannot open");
        return false;
    }
    
    return true;
}

void loggerTask::execute(){

    if(_device->isOpen()){
        unsigned short value[6] = {0x00, };
        
        value[0] = _device->getPrepheral("sensor-1")->read(0x01);
        value[1] = _device->getPrepheral("sensor-1")->read(0x03);
        value[2] = _device->getPrepheral("sensor-1")->read(0x05);

        value[3] = _device->getPrepheral("sensor-2")->read(0x01);
        value[4] = _device->getPrepheral("sensor-2")->read(0x03);
        value[5] = _device->getPrepheral("sensor-2")->read(0x05);

        if(_logfile.is_open()){
            for(unsigned short& val: value){
                _logfile << static_cast<int>(val);
                _logfile << ",";
            }
            _logfile << "\n";
        }
    }
}

void loggerTask::cleanup(){

    if(_logfile.is_open()){
        _logfile.close();
    }

    if(_device){
        delete _device;
    }
}
