
#ifndef _PLC_HPP_
#define _PLC_HPP_

#include "bus.hpp"
#include "protocol.hpp"
#include <3rdparty/spdlog/spdlog.h>


namespace oe::device::plc {

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
                switch(interface){
                    case Config::BUS:
                        _bus = static_cast<oe::bus::ethernet*>(instance);
                    break;
                }
            }

            bool readBit(uint16_t blockNo, uint16_t varlen, const char* address) { //read bit
                
            }
            
            uint8_t readByte(uint16_t blockNo, uint16_t varlen, const char* address) { //read byte
                using namespace net::protocol;

                XGTDedicated::xgt_frame_format_t packet;
                // packet.header = XGTDedicated::xgt_header_t(
                //     static_cast<uint8_t> XGTDedicated::cpu_info::XGI,
                //     static_cast<uint8_t> XGTDedicated::sof::CLIENT,
                //     getInvokeId(),
                //     0x0000,
                //     static_cast<uint8_t>(XGTDedicated::fenet_slot::SLOT0) | static_cast<uint8_t>(XGTDedicated::fenet_base::BASE0)
                // );
            }

            uint16_t readWord() { //read word

            }

            uint32_t readDword() {  //read double word

            }

            uint64_t readLword() { //read long word

            }

        private:
            uint16_t getInvokeId() { return ((_invokeId++)%0xffff)); }
        private:
            uint16_t _invokeId = 0x0000;

        private:
            oe::bus::ethernet* _bus = nullptr;

    }; //class
} //namespace 

#endif