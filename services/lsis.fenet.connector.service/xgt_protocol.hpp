

/**
 * @file    xgt_protocol.hpp
 * @brief   LSIS XGT Dedicated Protocol
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#include <stdint.h>
#include <cstring>
#include <vector>
#include <type_traits>
#include <3rdparty/json.hpp>
#include <openedge/core/protocol.hpp>
#include <3rdparty/sockpp/tcp_connector.h>

using namespace std;
using json = nlohmann::json;

namespace oe::bus::protocol {

    class XGTDedicated {
        public:
            enum class cpu_type_t : uint16_t {
                XGKR_CPUH = 0x0001,
                XGK_CPUS = 0x0002,
                XGI_CPUU = 0x0005,
            };
            enum class plc_redundancy_t : uint16_t {
                MASTER = 0x0000,
                SLAVE = 0x0040
            };
            enum class cpu_state_t : uint16_t {
                OK = 0x0000,
                ERROR = 0x0080
            };
            enum class system_state_t : uint16_t {
                RUN = 0x0100,
                STOP = 0x0200,
                ERROR = 0x0400,
                DEBUG = 0x0800
            };
            enum class cpu_info_t : uint8_t {
                XGK = 0xa0,
                XGB_MK = 0xb0,
                XGI = 0xa4,
                XGB_IEC = 0xb4,
                XGR = 0xa8,
            };
            enum class sof_t : uint8_t {
                CLIENT = 0x33,      //Client(HMI) to Server(PLC)
                SERVER = 0x11    //Server(PLC) to Client(HMI)
            };

            enum class fenet_slot_t : uint8_t {
                SLOT0 = 0x00,
                SLOT1 = 0x01,
                SLOT2 = 0x02,
                SLOT3 = 0x03,
                SLOT4 = 0x04,
                SLOT5 = 0x05,
                SLOT6 = 0x06,
                SLOT7 = 0x07,
                SLOT8 = 0x08,
                SLOT9 = 0x09,
                SLOT10 = 0x0a,
                SLOT11 = 0x0b,
                SLOT12 = 0x0c,
                SLOT13 = 0x0d,
                SLOT14 = 0x0e,
                SLOT15 = 0x0f,
            };

            enum class fenet_base_t : uint8_t {
                BASE0 = 0x00,
                BASE1 = 0x10,
                BASE2 = 0x20,
                BASE3 = 0x30,
                BASE4 = 0x40,
                BASE5 = 0x50,
                BASE6 = 0x60,
                BASE7 = 0x70,
                BASE8 = 0x80,
                BASE9 = 0x90,
                BASE10 = 0xa0,
                BASE11 = 0xb0,
                BASE12 = 0xc0,
                BASE13 = 0xd0,
                BASE14 = 0xe0,
                BASE15 = 0xf0,
            };

            enum class command_code_t : uint16_t {
                READ_REQUEST = 0x5400,
                READ_RESPONSE = 0x5500,
                WRITE_REQUEST = 0x5800,
                WRITE_RESPONSE = 0x5900
            };

            enum class datatype_t : uint16_t {
                BIT = 0x0000,
                BYTE = 0x0100,
                WORD = 0x0200,
                DWORD = 0x0300,
                LWORD = 0x0400,
                BLOCK = 0x1400
            };

            //generate XGT packet
            #define HEADER_SIZE 20
            #define BODY_SIZE   12
            vector<uint8_t> gen_read_block(const string& address, int count){
                vector<uint8_t> tmpPacket((int)HEADER_SIZE+(int)BODY_SIZE+(int)address.size());
                //tmpPacket.reserve(HEADER_SIZE+BODY_SIZE+address.size());

                _set_default_header(tmpPacket, cpu_info_t::XGI, sof_t::CLIENT, fenet_slot_t::SLOT0, fenet_base_t::BASE0);
                _set_default_body(tmpPacket, static_cast<uint16_t>(command_code_t::READ_REQUEST), static_cast<uint16_t>(datatype_t::BLOCK), 0x0001, address, (uint16_t)count);
                _packet_refine(tmpPacket);
            
                return tmpPacket;
            }

            uint8_t chksum(vector<uint8_t>& packet){ //checksum8 modulo 256 (sum of bytes % 256)
                uint8_t sum = 0x00;
                for(int i=0;i<HEADER_SIZE;i++)
                    sum += packet[i];
                return (sum&0xff);
            }

            void setParameters(string config){
                try {
                    this->proto_configs = json::parse(config);                    
                    spdlog::info("Protocol Parameters : {}", proto_configs.dump());
                }
                catch(const json::exception& e){
                    spdlog::info("Protocol Error : {}",e.what());
                }
            }

        protected:
            enum XGTProtocolIndex {
                PLC_INFO = 10,
                CPU_INFO = 12,
                SOF = 13,
                INVOKE_ID = 14,
                LENGTH = 16,
                FENET_POS = 18,
                CHECKSUM = 19,
                COMMAND = 20,
                DATATYPE = 22,
                BODY_RESERVED = 24,
                BLOCK_COUNT = 26,
                ADDRESS_LEN = 28,
                ADDRESS = 30
            };

            void _set_default_header(vector<uint8_t>& packet, cpu_info_t cpuinfo, sof_t sof, fenet_slot_t nslot, fenet_base_t nbase){
                uint8_t header[20] = {
                    0x4C, 0x53, 0x49, 0x53, 0x2d, 0x58, 0x47, 0x54, 0x00, 0x00, //company ID (LSIS-XGT)
                    0x00, 0x00, //plc info
                    0x00, //cpu info
                    0x00, //source of frame
                    0x00, 0x00, //invoke id
                    0x00, 0x00, //length (after header)
                    0x00,   //FEnet position
                    0x00 //Checksum
                };
                header[XGTProtocolIndex::CPU_INFO] = static_cast<uint8_t>(cpuinfo);
                header[XGTProtocolIndex::SOF] = static_cast<uint8_t>(sof);
                header[XGTProtocolIndex::FENET_POS] = static_cast<uint8_t>(nslot) | static_cast<uint8_t>(nbase);

                std::copy (header, header+sizeof(header), packet.begin());
                
            }

            void _set_default_body(vector<uint8_t>& packet, uint16_t cmd, uint16_t dtype, uint16_t nblocks, const string& address, uint16_t count){
                packet[XGTProtocolIndex::COMMAND] = cmd && 0xff;
                packet[XGTProtocolIndex::COMMAND+1] = cmd << 8;
                packet[XGTProtocolIndex::DATATYPE] = dtype & 0xff;
                packet[XGTProtocolIndex::DATATYPE+1] = dtype << 8;
                packet[XGTProtocolIndex::BODY_RESERVED] = 0x00; //don't care
                packet[XGTProtocolIndex::BODY_RESERVED+1] = 0x00;
                packet[XGTProtocolIndex::BLOCK_COUNT] = nblocks & 0xff;
                packet[XGTProtocolIndex::BLOCK_COUNT+1] = nblocks << 8;

                char address_array[address.size()];
                int len = address.size();
                std::copy(address.begin(), address.end(), address_array);

                packet[XGTProtocolIndex::ADDRESS_LEN] = len & 0xff;
                packet[XGTProtocolIndex::ADDRESS_LEN+1] = len << 8;
                for(int i=0; i<(int)address.size(); i++)
                    packet[XGTProtocolIndex::ADDRESS+i] = address_array[i];

                packet[XGTProtocolIndex::ADDRESS+address.size()] = count & 0xff;
                packet[XGTProtocolIndex::ADDRESS+address.size()+1] = count << 8;
            }

            void _packet_refine(vector<uint8_t>& packet){
                static uint16_t invokeId = 0x0000;

                packet[XGTProtocolIndex::INVOKE_ID] = invokeId & 0xff;
                packet[XGTProtocolIndex::INVOKE_ID+1] = invokeId << 8;
                packet[XGTProtocolIndex::LENGTH] = (packet.size()-HEADER_SIZE) & 0xff;
                packet[XGTProtocolIndex::LENGTH+1] = (packet.size()-HEADER_SIZE) << 8;

                packet[XGTProtocolIndex::CHECKSUM] = chksum(packet);

                invokeId++;
                if(invokeId>65535)
                    invokeId = 0x0000;
            }

        private:
            json proto_configs;

    }; //class


} //namespace 