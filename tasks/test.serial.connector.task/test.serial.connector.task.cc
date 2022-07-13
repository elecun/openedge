
#include "test.serial.connector.task.hpp"
#include <openedge/log.hpp>
#include <openedge/common/uart.hpp>

//static component instance that has only single instance
static test_serial_connector_task* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new test_serial_connector_task(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

test_serial_connector_task::test_serial_connector_task(){

}

test_serial_connector_task::~test_serial_connector_task(){

}

void test_serial_connector_task::execute(){

    if(_device->is_open()){

        //write
        const unsigned char packet[] = {'t', 'e', 's', 't', 0x0d};
        int bytes = _device->write(packet, sizeof(packet));
        console::info("{} byte(s) transferred", bytes);

        //read

    }
    else {
        console::warn("Device connection is not completed.");
    }

}

void test_serial_connector_task::stop(){

}

bool test_serial_connector_task::configure(){
    if(_device)
        _device->close();

    try {
        const json& prof = this->get_profile()->raw();
        if(prof.contains(PROFILE_CONFIGURATIONS_KEY)){
            json conf = prof[PROFILE_CONFIGURATIONS_KEY];
            if(conf.contains("serial")){
                string port = conf["serial"]["device"].get<string>();
                unsigned int baud = conf["serial"]["baudrate"].get<unsigned int>();

                _device = new bus::sync_uart(port.c_str(), baud);
                if(_device->open()){
                    console::info("[{}] open device : {}({})", this->get_name(), port, baud);
                }
                else {
                    console::warn("Sync UART bus interface is closed.");
                }
            }
        }
    }
    catch(const json::exception& e){
        console::error("Profile read/access error : {}", e.what());
    }
    return true;
}

void test_serial_connector_task::cleanup(){
    if(_device){
        _device->close();
        delete _device;
    }
}

void test_serial_connector_task::pause(){
    
}

void test_serial_connector_task::resume(){
    
}

void test_serial_connector_task::on_request(std::any data){

}
