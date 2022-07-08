/**
 * @file uloray.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief sysbase ulory device
 * @version 0.1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_SUPPORT_DEVICE_SYSTEMBASE_ULORY_HPP_
#define _OPENEDGE_SUPPORT_DEVICE_SYSTEMBASE_ULORY_HPP_

#include <vector>
#include <array>
#include <algorithm>
#include <string>
#include <sstream>
#include <openedge/common/bus.hpp> //for sync_bus
#include <openedge/log.hpp>

using namespace oe;
using namespace std;

namespace oe::device {
    namespace systembase {      

        class ulory {
            public:
                ulory(bus::sync_bus* bus):_bus(bus){ }
                virtual ~ulory() = default;

                typedef struct _version {
                    int _major, _minor, _patch;
                    string str(){
                        return fmt::format("{}.{}.{}", _major, _minor, _patch);
                    }
                    _version(int mj, int mi, int pt):_major(mj),_minor(mi),_patch(pt){}
                } version;

                /**
                 * @brief protocol version response
                 * @return version
                 */
                version get_version(){
                    if(_bus){
                        if(_bus->is_open()){
                            const unsigned char packet[] = {'w', 'c', 'v', '*'};
                            _bus->write(packet, sizeof(packet));
                        }
                    }
                    else {
                        console::warn("Waterlinked M64 interface is not working...");
                    }
                    return version(0,0,1);
                }

                void reset(){
                    if(_bus){
                        if(_bus->is_open()){
                            //const unsigned char* packet = "at&v";
                            //_bus->write(packet, sizeof(packet));
                        }
                    }
                }

                string help(){
                    const char command[] = {'a', 't', '&', 'v', 0x0d}; //with carriage return
                    bool rc = transaction(command, 5000);
                }

                string get_parameters(){
                    const char* command = "at&v";
                    int byte = transfer(command);
                    if(byte!=(int)strlen(command))
                        return string("");
                }
            private:
                /**
                 * @brief transfer the dedicated AT command for uLory
                 * 
                 * @param command string AT command
                 * @return int transferred bytes
                 */
                int transfer(const char* command){
                    if(_bus){
                        if(_bus->is_open()){
                            const unsigned char* ucommand = reinterpret_cast<const unsigned char*>(command);
                            return _bus->write(ucommand, (int)strlen((const char*)command));
                        }
                        else {
                            console::warn("Bus interface is closed.");
                        }
                    }
                    else {
                        console::warn("Invalid bus interface. It cannot be transferred.");
                    }
                    return 0;
                }

                bool transaction(const char* command, unsigned int timeout, unsigned int space = 100){
                    if(_bus){
                        if(_bus->is_open()){

                            /* write command */
                            const unsigned char* ucommand = reinterpret_cast<const unsigned char*>(command);
                            int wbyte = _bus->write(ucommand, (int)strlen((const char*)command));
                            if(wbyte!=(int)strlen(command)){
                                console::warn("Wrong transfer : {}bytes transferred", wbyte);
                                return false;
                            }

                            /* receive data until  */
                            #define MAX_RCV_BUFFER  2048
                            unsigned char* rcvdata = new unsigned char[MAX_RCV_BUFFER];
                            int rbyte = _bus->read_until(rcvdata, sizeof(unsigned char)*MAX_RCV_BUFFER, timeout, space);
                            if(rbyte>0)
                                console::info("{}bytes received", rbyte);
                            
                            // const char* data = reinterpret_cast<const char*>(rcvdata);
                            // std::cout << data << endl;
                            // string strdata {data};
                            // console::info("received : {}", strdata);
                            delete []rcvdata;

                            return true;
                        }
                        else {
                            console::warn("Bus interface is closed.");
                        }
                    }
                    else {
                        console::warn("Invalid bus interface. It cannot be transferred.");
                    }
                    return false;
                }

            private:
                bus::sync_bus* _bus = nullptr;
            
        }; /* class */

    } /* namespace waterlink */
} /* namespace device */

#endif