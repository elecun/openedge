
#include "uvlc.ag.control.task.hpp"
#include <openedge/log.hpp>
#include <openedge/device/bus.hpp>
#include <openedge/device/controller.hpp>
#include <support/device/dkm_dx3000.hpp>
#include <openedge/device/gpio.hpp>
#include <support/device/am335x/gpio.hpp>
#include <string>

using namespace oe::support;

//static component instance that has only single instance
static uvlcAGControlTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new uvlcAGControlTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool uvlcAGControlTask::configure(){
    //spdlog::info("create push");
    //_push = zmq::zsock_new_push("inproc://example");

    //clear controller instance
    if(_controller){
        _controller->close();
        delete _controller;
        _controller = nullptr;
    }
        
    //read configurations
    json config = json::parse(getProfile()->get("configurations"));

    //read dx3000 parameters and create instance
    if(config.find("dx3000")!=config.end()){
        json dx3000_param = config["dx3000"];
        if(dx3000_param.find("access")!=dx3000_param.end()){
            string access_port = dx3000_param["access"].get<string>();
            int slave_id = dx3000_param["slave_id"].get<int>();
            _controller = new oe::support::DKM_DX3000(slave_id, access_port.c_str());
            if(!_controller->open())
                return false;
            else {
                DKM_DX3000* motor = dynamic_cast<DKM_DX3000*>(_controller);
                if(motor){
                    motor->set_parameter(DKM_DX3000::PARAMETER::SET_ID, slave_id);
                }
            }
        }
    }
    else {
        console::error("cannot read configurations of DX3000 in profile");
        return false;
    }

    //read gpio 1 parameters
    if(config.find("gpio_1")!=config.end()){
        json gpio_param = config["gpio_1"];
        
        if(gpio_param.find("pin")!=gpio_param.end() && gpio_param.find("direction")!=gpio_param.end()){
            int pin = gpio_param["pin"].get<int>();
            string dir = gpio_param["direction"].get<string>();
            _limitsw.insert(std::pair<int,device::gpio*>(pin, new oe::support::am335xrt::gpio(pin, dir.c_str())));        
            
            if(!_limitsw[pin]->open())
                return false;
        }
        else {
            console::error("Not enough parameters..");
            return false;
        }
    }
    else {
        console::error("cannot read configurations of the system GPIO in profile");
        return false;
    }

    //read gpio 2 parameters
    if(config.find("gpio_2")!=config.end()){
        json gpio_param = config["gpio_2"];
        
        if(gpio_param.find("pin")!=gpio_param.end() && gpio_param.find("direction")!=gpio_param.end()){
            int pin = gpio_param["pin"].get<int>();
            string dir = gpio_param["direction"].get<string>();
            _limitsw.insert(std::pair<int,device::gpio*>(pin, new oe::support::am335xrt::gpio(pin, dir.c_str())));        
            
            if(!_limitsw[pin]->open())
                return false;
        }
        else {
            console::error("Not enough parameters..");
            return false;
        }
    }
    else {
        console::error("cannot read configurations of the system GPIO in profile");
        return false;
    }


    return true;
    
}

void uvlcAGControlTask::execute(){
    //zstr_send(_push, "Hello, World");
    // spdlog::info("push message");
    console::info("move motor");

    static bool started = false;
    DKM_DX3000* motor = dynamic_cast<DKM_DX3000*>(_controller);

    for(auto itr = _limitsw.begin(); itr != _limitsw.end(); itr++){
        switch(itr->second->get_pin()){
            case 48: {
                if(itr->second->read()==device::gpio::LEVEL::LOW)
                    if(motor) { 
                        motor->move(DKM_DX3000::DIRECTION::CCW); 
                        console::info("limit reached");
                    }
            } break;
            case 60: {
                if(itr->second->read()==device::gpio::LEVEL::LOW)
                    if(motor){ 
                        motor->move(DKM_DX3000::DIRECTION::CW); 
                        console::info("limit reached");
                    }
            } break;
        }
    }

    if(!started){
        if(motor){ 
            motor->move(DKM_DX3000::DIRECTION::CW); 
            console::info("motor stareted");
        }
        started = true;
    }
    
}

void uvlcAGControlTask::cleanup(){
    DKM_DX3000* motor = dynamic_cast<DKM_DX3000*>(_controller);
    if(motor)
        motor->stop();
    _controller->close();

    for(auto itr = _limitsw.begin(); itr != _limitsw.end(); itr++){
        itr->second->close();
    }

    //zmq::zsock_destroy(&_push);
}

void uvlcAGControlTask::pause(){

}

void uvlcAGControlTask::resume(){

}