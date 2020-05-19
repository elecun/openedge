

/**
 * @file    xgt.protocol.hpp
 * @brief   LSIS XGT Dedicated Protocol
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _SERVICE_LSIS_FENET_XGT_PROTOCOL_HPP_
#define _SERVICE_LSIS_FENET_XGT_PROTOCOL_HPP_

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
                READ_REQUEST = 0x0054,
                READ_RESPONSE = 0x0055,
                WRITE_REQUEST = 0x0058,
                WRITE_RESPONSE = 0x0059
            };

            enum class datatype_t : uint16_t {
                BIT = 0x0000,
                BYTE = 0x0001,
                WORD = 0x0002,
                DWORD = 0x0003,
                LWORD = 0x0004,
                BLOCK = 0x0014
            };

            enum class error_state_t : uint16_t {
                NO_ERROR = 0x0000
            };

            enum class error_info_t : uint16_t {
                NOT_ERROR = 0x0001
            };

            vector<uint8_t> gen_read_block(const string& address, int count);
            uint8_t chksum(vector<uint8_t>& packet); //checksum8 modulo 256 (sum of bytes % 256)
            void setParameters(string config);

            uint16_t check_response_error(const uint8_t* data, int size);

        protected:
            void _set_default_header(vector<uint8_t>& packet, cpu_info_t cpuinfo, sof_t sof, fenet_slot_t nslot, fenet_base_t nbase);
            void _set_default_body(vector<uint8_t>& packet, uint16_t cmd, uint16_t dtype, uint16_t nblocks, const string& address, uint16_t count);
            void _packet_refine(vector<uint8_t>& packet);

        private:
            json proto_configs;

    }; //class


} //namespace 

#endif