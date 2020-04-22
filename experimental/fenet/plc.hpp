
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
                net::protocol::XGTDedicated::xgt_frame_format packet;
                packet.header = net::protocol::XGTDedicated::xgt_header(
                    0x0000, //don't care
                    net::protocol::XGTDedicated::cpu_info_t::XGI,
                    _invokeId++,
                    static_cast<uint8_t>(net::protocol::XGTDedicated::fenet_slot_t::SLOT0) | static_cast<uint8_t>(net::protocol::XGTDedicated::fenet_base_t::BASE0)
                );
                spdlog::info("{}", static_cast<uint8_t*>(&packet.header));
            }

            uint16_t readWord() { //read word

            }

            uint32_t readDword() {  //read double word

            }

            uint64_t readLword() { //read long word

            }
        private:
            uint16_t _invokeId = 0x0000;

        private:
            oe::bus::ethernet* _bus = nullptr;

    }; //class
} //namespace 

#endif