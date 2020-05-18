

/**
 * @file    xgt_protocol.hpp
 * @brief   LSIS XGT Dedicated Protocol
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#include <stdint.h>
#include <cstring>
#include <type_traits>
#include <3rdparty/json.hpp>
#include <openedge/core/protocol.hpp>

using namespace std;
using json = nlohmann::json;

namespace oe::bus::protocol {

    class XGTDedicated : public oe::core::iProtocolRaw {
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

            typedef uint16_t plc_info_t;
            typedef struct xgt_header_t {
                uint8_t     companyId[10] = {0x4C, 0x53, 0x49, 0x53, 0x2d, 0x58, 0x47, 0x54, 0x00, 0x00 }; //'LSIS-XGT'
                uint16_t    plcInfo = static_cast<uint16_t>(cpu_type_t::XGI_CPUU) | static_cast<uint16_t>(plc_redundancy_t::MASTER) | 
                                      static_cast<uint16_t>(cpu_state_t::OK) | static_cast<uint16_t>(system_state_t::STOP);
                cpu_info_t    cpuInfo = cpu_info_t::XGI;
                sof_t       sof = sof_t::CLIENT;    //source of frame
                uint16_t    invokeId = 0x0000;
                uint16_t    length; //byte length of applicaiton instruction
                uint8_t     fenet_pos = static_cast<uint8_t>(fenet_slot_t::SLOT0) | static_cast<uint8_t>(fenet_base_t::BASE0);
                uint8_t     checksum;
                xgt_header_t() {

                }
                xgt_header_t& operator=(const xgt_header_t& other){
                    memcpy(this, &other, sizeof(xgt_header_t));
                }
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
            
            #define MAX_FRAME_SIZE    512
            typedef struct xgt_frame_format_t {
                xgt_header_t    header;
                uint16_t        command;
                uint16_t        datatype;
                uint8_t         data[MAX_FRAME_SIZE-sizeof(header)-sizeof(command)-sizeof(datatype)];
                xgt_frame_format_t& operator=(const xgt_frame_format_t& other){
                    memcpy(this, &other, sizeof(xgt_frame_format_t));
                }
            };
            #undef MAX_FRAME_SIZE

            void request(datatype_t type){
                _frame.header.sof = sof_t::CLIENT;
            }

            void setParameters(string config){
                try {
                    this->proto_configs = json::parse(config);
                    //_generate_header(frame, this->proto_configs);
                    
                    
                    spdlog::info("Protocol Parameters : {}", proto_configs.dump());
                }
                catch(const json::exception& e){
                    spdlog::info("{Protocol Error : {}",e.what());
                }
            }

            void send(){
                spdlog::info("send");
            }

            void receive() {
                spdlog::info("receive");
            }

        protected:
            void _generate_header(xgt_frame_format_t& _frame, json& config){
                //company identity
                // _frame.header.companyId {0x4C, 0x53, 0x49, 0x53, 0x2d, 0x58, 0x47, 0x54, 0x00, 0x00 };

                // //plc info & sof
                // if(proto_configs.find("mode")!=proto_configs.end()){
                //     if(!proto_configs["mode"].get<string>().compare("client")){ //client mode
                //         this->frame.header.plcInfo = 0x0000;
                //         this->frame.header.sof = sof_t::CLIENT;
                //     }
                //     else { //server mode
                //         this->frame.header.plcInfo = static_cast<uint16_t>(cpu_type_t::XGI_CPUU) | static_cast<uint16_t>(plc_redundancy_t::MASTER) | static_cast<uint16_t>(cpu_state_t::OK) | static_cast<uint16_t>(system_state_t::STOP);
                //         this->frame.header.sof = sof_t::SERVER;
                //     }
                // }

                // //cpu info
                // if(proto_configs.find("cpu_info")!=proto_configs.end()){
                //     if(!proto_configs["cpu_info"].compare("xgi"))
                //         _farme.header.cpuInfo = cpu_info_t::XGI;
                // }

                // //slot & base index
                // if(proto_configs.find("slot_index")!=proto_configs.end() && proto_configs.find("base_index")!=proto_configs.end()){
                //     int slot_idx = proto_configs["slot_index"].get<int>();
                //     int base_idx = proto_configs["base_index"].get<int>();
                //     this->frame.header.fenet_pos = static_cast<uint8_t>(slot_idx) | (static_cast<uint8_t>(base_idex)<<8);
                // }
            }

        private:
            json proto_configs;
            xgt_frame_format_t _frame;

    }; //class


} //namespace 