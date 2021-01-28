

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

    // //create general device instance
    // _device = new oe::generalDevice(new oe::busI2C(dev.c_str()));

    // //getting general device channel information
    // json _device = json::parse(getProfile()->get("device"));
    // vector<string> _channels = _device["channels"].get<vector<string>>();

    // for(string& ch: _channels){
    //     unsigned char address = static_cast<unsigned char>(_device[ch]["address"].get<int>());
    //     vector<int> _registers = _device[ch]["registers"].get<vector<int>>();

    //     //_devices.emplace_back(new oe::generalDevice(new oe::busI2C());
    // }

    _device = new oe::device("/dev/i2c-2");
    if(_device->open()){
        _device->addPrepheral(new oe::prepheral("sensor-1"));
    }
    else {
        spdlog::error("device cannot open, so cannot add prepherals");
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
