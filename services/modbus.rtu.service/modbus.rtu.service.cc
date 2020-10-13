

#include "modbus.rtu.service.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <chrono>
#include <string>
#include <3rdparty/json.hpp>
#include <3rdparty/libmodbus/modbus.h>
#include <3rdparty/libmodbus/modbus-rtu.h>

using namespace std::chrono;
using json = nlohmann::json;
using namespace std;

//static service
static modbusRtuService* _instance = nullptr;
oe::core::iService* create(){ if(!_instance) _instance = new modbusRtuService(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

//instance
static modbus_t* _modbus = nullptr;

modbusRtuService::modbusRtuService()
{
}

modbusRtuService::~modbusRtuService(){
    closeService();
}

bool modbusRtuService::closeService(){

    if(_modbus){
        modbus_close(_modbus);
        modbus_free(_modbus);
    }

    spdlog::info("close modbus RTU Service");

    return true;
}

bool modbusRtuService::initService(const char* config){
    assert(config!=nullptr);

    spdlog::info("service config : {}", config);

    //read configuration for fenet connection
    try {
        json conf;
        conf = json::parse(config);
        if(conf.find("connection")==conf.end())
            return false;

            #define CONNECTION  conf["connection"]
            #define INFO  conf["info"]

        if(CONNECTION.find("port")!=CONNECTION.end())
            _port = CONNECTION["port"].get<std::string>(); //device port
        if(CONNECTION.find("baudrate")!=CONNECTION.end())
            _baudrate = CONNECTION["baudrate"].get<unsigned int>(); //baudrate

        if(INFO.find("slave_id")!=INFO.end())
            _slave_id = static_cast<unsigned char>(INFO["slave_id"].get<int>());

        spdlog::info("Connection : {}:{}", _port, _baudrate);
        spdlog::info("Slave ID : {}", (int)_slave_id);

        if(!_modbus){
            _modbus = modbus_new_rtu(_port.c_str(), _baudrate, 'N', 8, 1);
            if(!_modbus){
                spdlog::warn("Unable to create the modbus context");
                return false;
            }

            modbus_set_slave(_modbus, _slave_id);

            if(modbus_connect(_modbus)==-1){
                spdlog::error("Unable to connect {}", modbus_strerror(errno));
                modbus_free(_modbus);
                return false;
            }
        }

    }
    catch(const json::exception& e){
        spdlog::error("service profile : {}", e.what());
    }

    //add service
    service->Add("read_holding_registers", jsonrpccxx::GetHandle(&modbusRtuService::read_holding_registers, *this), {"address", "size"});
    service->Add("write_holding_register", jsonrpccxx::GetHandle(&modbusRtuService::write_holding_register, *this), {"address", "data"});

    return true;
}

vector<uint16_t> modbusRtuService::read_holding_registers(const uint16_t address, int size){

    vector<uint16_t> registers;
    try {

        uint16_t* read_registers = (uint16_t *)malloc(size * sizeof(uint16_t));
        int read = modbus_read_registers(_modbus, address, size, read_registers);

        spdlog::info("Read Holding Registers : {}", read);

        registers.assign(read_registers, read_registers+size);

    }
    catch(json::exception& e){
        spdlog::error("{}", e.what());
    }

    return registers;
}

bool modbusRtuService::write_holding_register(const uint16_t address, uint16_t data){

    try {

    }
    catch(json::exception& e){
        spdlog::error("{}", e.what());
    }

}

// unsigned char modbusRtuService::read_bits(const unsigned char address /*json*/){

//     try {
//         if(_modbus){
//             uint8_t read_register;
//             modbus_read_bits() (_modbus, address, 1, read_register); //single byte
//         }
        
//     }
//     catch(json::exception& e){
//         spdlog::error("{}", e.what());
//     }
//     return true;
// }

// void modbusRtuService::write(const unsigned char address, unsigned char data /*json*/){

// }