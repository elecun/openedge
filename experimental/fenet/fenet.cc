

#include "protocol.hpp"
#include "plc.hpp"
#include "bus.hpp"
#include <iostream>
#include "socket.hpp"

using namespace oe;
using namespace std;

int main(int argc, char* argv[])
{
    //PLC configuration, it will be performed
    //framework에서는 서비스 configuration에 따라 자동 설정되도록 변경해야 할것.
    //PLC는 sub module로 bus interface를 가지고,
    //Bus interface는 특정 protocol을 가지도록 함.
    device::plc::generalPLC* _plc = new device::plc::generalPLC();                  //target plc device to connect
    net::protocol::XGTDedicated* _protocol = new net::protocol::XGTDedicated();     //protocol to be used
    bus::ethernet* _bus = new bus::ethernet(_protocol, "192.168.11.13", 2004);      //communication interface to be used

    _plc->setConfig(device::plc::generalPLC::Config::BUS, _bus);

    cout << "Connecting to PLC...";
    if(_plc->open()){
        cout << "OK" << endl; 
        _plc->readByte();
    }
    else 
        cout << "Failed" << endl;;
    

    _plc->close();

    delete _plc;
    delete _bus;
    delete _protocol;


    return 0;
}