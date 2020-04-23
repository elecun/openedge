

/**
 * @file    plc.hpp
 * @brief   general PLC class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */


#ifndef _OPENEDGE_DEVICE_PLC_HPP_
#define _OPENEDGE_DEVICE_PLC_HPP_


namespace oe::device {



    class generalPLC {
        public:
            enum class Config : uint8_t {
                BUS = 1
            };
        public:
            generalPLC() {

            }
            ~generalPLC() {

            }

            bool open() {
                return true;
            }

            bool open(const char* ipaddr, const int port){
                return true;
            }

            void close() {

            }

            void setConfig(Config interface, void* instance){

            }

            void read() {
                
            }

    }; //class
} //namespace


#endif