
#include "xgb_fenet.hpp"
#include "cpu_info.hpp"

XGBFastEthernet::XGBFastEthernet(){

}

XGBFastEthernet::XGBFastEthernet(const char* ip4v_addr, int port){

}

bool XGBFastEthernet::open(){
    return false;
}

void XGBFastEthernet::close(){

}

int XGBFastEthernet::read(uint8_t* data, int len){
    return 0;
}

int XGBFastEthernet::write(const uint8_t* data, int len){
    return 0;
}

void XGBFastEthernet::flush(){

}


vector<uint8_t> XGBFastEthernet::command(vector<uint8_t>& cmd){
    vector<uint8_t> packet(cmd.size()+20);  //20bytes header
    //here, copy company id [0-9]
    //here, plc info [10-11]
    uint8_t cpuInfo;
    switch (cpuInfo)
    {
        case oe::plc::lsis::CpuInfo::XGK:
            packet[12] = 0xA0;
            break;
        case oe::plc::lsis::CpuInfo::XGI:
            packet[12] = 0xA4;
            break;
        case oe::plc::lsis::CpuInfo::XGR:
            packet[12] = 0xA8;
            break;
        case oe::plc::lsis::CpuInfo::XGB_MK:
            packet[12] = 0xB0;
            break;
        case oe::plc::lsis::CpuInfo::XGB_IEC:
            packet[12] = 0xB4;
            break;
        default:
            break;
    }
    packet[13] = 0x33; //hmi to plc
    //here, invoke ID (2bytes)
    //here, length (2bytes)
    //here, fenet position (1byte)
    //here, reserved (1byte)

}


