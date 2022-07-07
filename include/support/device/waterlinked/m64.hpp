/**
 * @file m64.hpp
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Waterlinked's M64 Acoustic Modem Library
 * @version 0.1
 * @date 2022-05-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_SUPPORT_DEVICE_WATERLINKED_M64_HPP_
#define _OPENEDGE_SUPPORT_DEVICE_WATERLINKED_M64_HPP_

#include <vector>
#include <array>
#include <algorithm>
#include <string>
#include <sstream>
#include <openedge/common/bus.hpp> //for sync_bus

using namespace oe;
using namespace std;

namespace oe::device {
    namespace waterlink {

        #define _M64_START_     'w'
        #define _M64_COMMAND_   'c'
        #define _M64_RESPONSE_  'r'

        /* Protocol Definitions */
        #define _M64_SOP_       'w'
        #define _M64_EOP_       '\n'
        #define _M64_DIR_CMD_   'c'
        #define _M64_DIR_RESP_  'r'
        #define _M64_CHECKSUM_  '*'
        #define _M64_CMD_GET_VERSION_   'v'
        #define _M64_CMD_GET_PAYLOAD_SIZE_   'n'
        #define _M64_CMD_GET_BUFFER_LENGTH_  'l'
        #define _M64_CMD_GET_DIAGNOSTIC_    'd'
        #define _M64_CMD_GET_SETTINGS_       'c'
        #define _M64_CMD_SET_SETTINGS_       's'
        #define _M64_CMD_QUEUE_PACKET_       'q'
        #define _M64_CMD_FLUSH_              'f'
        #define _M64_RESP_GOT_PACKET_        'p'

        static const uint8_t _M64_LOOKUP_TABLE_[] = {
            0x00U,0x07U,0x0EU,0x09U,0x1CU,0x1BU,0x12U,0x15U,
            0x38U,0x3FU,0x36U,0x31U,0x24U,0x23U,0x2AU,0x2DU,
            0x70U,0x77U,0x7EU,0x79U,0x6CU,0x6BU,0x62U,0x65U,
            0x48U,0x4FU,0x46U,0x41U,0x54U,0x53U,0x5AU,0x5DU,
            0xE0U,0xE7U,0xEEU,0xE9U,0xFCU,0xFBU,0xF2U,0xF5U,
            0xD8U,0xDFU,0xD6U,0xD1U,0xC4U,0xC3U,0xCAU,0xCDU,
            0x90U,0x97U,0x9EU,0x99U,0x8CU,0x8BU,0x82U,0x85U,
            0xA8U,0xAFU,0xA6U,0xA1U,0xB4U,0xB3U,0xBAU,0xBDU,
            0xC7U,0xC0U,0xC9U,0xCEU,0xDBU,0xDCU,0xD5U,0xD2U,
            0xFFU,0xF8U,0xF1U,0xF6U,0xE3U,0xE4U,0xEDU,0xEAU,
            0xB7U,0xB0U,0xB9U,0xBEU,0xABU,0xACU,0xA5U,0xA2U,
            0x8FU,0x88U,0x81U,0x86U,0x93U,0x94U,0x9DU,0x9AU,
            0x27U,0x20U,0x29U,0x2EU,0x3BU,0x3CU,0x35U,0x32U,
            0x1FU,0x18U,0x11U,0x16U,0x03U,0x04U,0x0DU,0x0AU,
            0x57U,0x50U,0x59U,0x5EU,0x4BU,0x4CU,0x45U,0x42U,
            0x6FU,0x68U,0x61U,0x66U,0x73U,0x74U,0x7DU,0x7AU,
            0x89U,0x8EU,0x87U,0x80U,0x95U,0x92U,0x9BU,0x9CU,
            0xB1U,0xB6U,0xBFU,0xB8U,0xADU,0xAAU,0xA3U,0xA4U,
            0xF9U,0xFEU,0xF7U,0xF0U,0xE5U,0xE2U,0xEBU,0xECU,
            0xC1U,0xC6U,0xCFU,0xC8U,0xDDU,0xDAU,0xD3U,0xD4U,
            0x69U,0x6EU,0x67U,0x60U,0x75U,0x72U,0x7BU,0x7CU,
            0x51U,0x56U,0x5FU,0x58U,0x4DU,0x4AU,0x43U,0x44U,
            0x19U,0x1EU,0x17U,0x10U,0x05U,0x02U,0x0BU,0x0CU,
            0x21U,0x26U,0x2FU,0x28U,0x3DU,0x3AU,0x33U,0x34U,
            0x4EU,0x49U,0x40U,0x47U,0x52U,0x55U,0x5CU,0x5BU,
            0x76U,0x71U,0x78U,0x7FU,0x6AU,0x6DU,0x64U,0x63U,
            0x3EU,0x39U,0x30U,0x37U,0x22U,0x25U,0x2CU,0x2BU,
            0x06U,0x01U,0x08U,0x0FU,0x1AU,0x1DU,0x14U,0x13U,
            0xAEU,0xA9U,0xA0U,0xA7U,0xB2U,0xB5U,0xBCU,0xBBU,
            0x96U,0x91U,0x98U,0x9FU,0x8AU,0x8DU,0x84U,0x83U,
            0xDEU,0xD9U,0xD0U,0xD7U,0xC2U,0xC5U,0xCCU,0xCBU,
            0xE6U,0xE1U,0xE8U,0xEFU,0xFAU,0xFDU,0xF4U,0xF3U
        };

        /**
         * @brief   Water Linked Modem Protocol Parser
         */
        class m64_parser {
            public:
                m64_parser() = default;
                virtual ~m64_parser() = default;

                vector<unsigned char> do_format_checksum(unsigned char checksum){
                    
                }

                void checksum_for_buffer(unsigned char* data, int len){
                    auto crc = this->crc8(data, len);

                }

                void do_frame_fragments(){

                }

                void do_frame(){

                }

                void parse(string data){
                    
                }

            private:
                /**
                 * @brief Calculate CRC8
                 * 
                 * @param data data array to send
                 * @param len number of data(uint8)
                 * @return uint8_t checksum
                 */
                uint8_t crc8(unsigned char* data, uint len){
                    uint8_t chksum = 0;
                    while(len>0){
                        chksum = _M64_LOOKUP_TABLE_[*data^chksum];
                        data++;
                        len--;
                    }
                    return chksum;
                }

        }; /* class */

        class m64 {
            public:
                m64(bus::sync_bus* bus):_bus(bus){ }
                virtual ~m64() = default;

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

            private:

                /**
                 * @brief Is the given byte an eop
                 * 
                 * @return true 
                 * @return false 
                 */
                bool is_eop(uint8_t* ch){
                    return true;
                }

                /**
                 * @brief Is the given byte an checksum char
                 * 
                 * @param ch 
                 * @return true 
                 * @return false 
                 */
                bool is_checksum(uint8_t ch){
                    if(ch==static_cast<uint8_t>(_M64_CHECKSUM_))
                        return true;
                    return false;
                }
                
                /**
                 * @brief Is this an acknowledge
                 * 
                 * @param ch character
                 * @return true 
                 * @return false 
                 */
                bool is_ack(unsigned char ch){
                    return false;
                }

                /**
                 * @brief Detect if this is the start of a binary payload and return the number of bytes it contains
                 * 
                 * @param packet 
                 * @return int8_t 
                 */
                int8_t get_binary_payload_size(string packet){
                    if(packet[0] != _M64_SOP_)
                        return -1;
                    if(packet[2]!=_M64_CMD_QUEUE_PACKET_ || packet[2]!=_M64_RESP_GOT_PACKET_)
                        return -1;

                    std::stringstream ss(packet);
                    vector<string> fragments;
                    while(ss.good()){
                        string substr;
                        getline(ss, substr, ',');
                        fragments.push_back(substr);
                    }

                    if(fragments.size()<2)
                        return -1;
                }

            private:
                bus::sync_bus* _bus = nullptr;
            
        }; /* class */

    } /* namespace waterlink */
} /* namespace device */

#endif