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
                    if(_bus){
                        if(_bus->is_open()){
                            const char* command = "at&h";
                            const unsigned char* ucommand = reinterpret_cast<const unsigned char *>(command);
                            _bus->write(ucommand, (int)strlen((const char*)command));
                        }
                        else {
                            console::error("uLory bus interface is closed.");
                        }
                    }
                    return string("");
                }

            private:
                bus::sync_bus* _bus = nullptr;
            
        }; /* class */

    } /* namespace waterlink */
} /* namespace device */

#endif