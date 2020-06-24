

#include "xgt.protocol.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <algorithm>

namespace oe::bus::protocol {

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
        ADDRESS = 30,
        ERROR_STATE = 26,
        ERROR_CODE = 28
    };

//generate XGT packet
    #define HEADER_SIZE 20
    #define BODY_SIZE   12
    vector<uint8_t> XGTDedicated::gen_read_block(const string& address, int count){
        vector<uint8_t> tmpPacket((int)HEADER_SIZE+(int)BODY_SIZE+(int)address.size()); //full length

        _set_default_header(tmpPacket, XGTDedicated::_cpu_info, sof_t::CLIENT, XGTDedicated::_slot_index, XGTDedicated::_base_index);
        _set_default_body(tmpPacket, static_cast<uint16_t>(command_code_t::READ_REQUEST), static_cast<uint16_t>(datatype_t::BLOCK), 0x0001, address, (uint16_t)count);
        _packet_refine(tmpPacket);
    
        return tmpPacket;
    }

    uint8_t XGTDedicated::chksum(vector<uint8_t>& packet){ //checksum8 modulo 256 (sum of bytes % 256)
        uint8_t sum = 0x00;
        for(int i=0;i<HEADER_SIZE-1;i++)
            sum += packet[i];
        return (sum&0xff);
    }

    void XGTDedicated::setParameters(string config){
        try {
            _proto_configs = json::parse(config);
            //config cpu info
            if(_proto_configs.find("cpu_info")!=_proto_configs.end())
                _cpu_info = _set_cpuInfo(_proto_configs["cpu_info"].get<std::string>());
            //config slot index
            if(_proto_configs.find("slot_index")!=_proto_configs.end())
                _slot_index = _set_slotIndex(_proto_configs["slot_index"].get<int>());
            //config base index
            if(_proto_configs.find("base_index")!=_proto_configs.end())
                _base_index = _set_baseIndex(_proto_configs["base_index"].get<int>());

            spdlog::info("Protocol Parameters : {}", _proto_configs.dump());
        }
        catch(const json::exception& e){
            spdlog::info("Protocol Error : {}",e.what());
        }
    }

    void XGTDedicated::_set_default_header(vector<uint8_t>& packet, cpu_info_t cpuinfo, sof_t sof, fenet_slot_t nslot, fenet_base_t nbase){
        uint8_t header[20] = {0,};
        uint8_t companyid[] = {0x4C, 0x53, 0x49, 0x53, 0x2d, 0x58, 0x47, 0x54, 0x00, 0x00};
        memcpy(header, companyid, sizeof(companyid));
    
        header[XGTProtocolIndex::CPU_INFO] = static_cast<uint8_t>(cpuinfo);
        header[XGTProtocolIndex::SOF] = static_cast<uint8_t>(sof);
        header[XGTProtocolIndex::FENET_POS] = static_cast<uint8_t>(nslot) | static_cast<uint8_t>(nbase);

        std::copy(header, header+sizeof(header), packet.begin());
        
    }

    void XGTDedicated::_set_default_body(vector<uint8_t>& packet, uint16_t cmd, uint16_t dtype, uint16_t nblocks, const string& address, uint16_t count){
        packet[XGTProtocolIndex::COMMAND] = cmd & 0xff;
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

    void XGTDedicated::_packet_refine(vector<uint8_t>& packet){
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

    uint16_t XGTDedicated::check_response_error(const uint8_t* data, int size){

        uint16_t errorcode = 0x0000;
        if(!(data[XGTProtocolIndex::ERROR_STATE] && 0x00) && !(data[XGTProtocolIndex::ERROR_STATE+1] && 0x00)){
            errorcode = (uint16_t)(data[XGTProtocolIndex::ERROR_CODE+1]<<8 | data[XGTProtocolIndex::ERROR_CODE]);
        }

        return errorcode;
    }

    xgt_errorcode_t XGTDedicated::checkError(const uint8_t* data, int size){
        xgt_errorcode_t errorcode = xgt_errorcode_t::NORMAL_OPERATION;
        if(!(data[XGTProtocolIndex::ERROR_STATE]==0x00) && !(data[XGTProtocolIndex::ERROR_STATE+1]==0x00)){
            errorcode = static_cast<xgt_errorcode_t>((uint16_t)(data[XGTProtocolIndex::ERROR_CODE+1]<<8 | data[XGTProtocolIndex::ERROR_CODE]));
        }
        return errorcode;
    }

    XGTDedicated::cpu_info_t XGTDedicated::_set_cpuInfo(const string& param){
        string cpu = param;
        std::for_each(cpu.begin(), cpu.end(), [](char& c) {
        c = ::tolower(c);
        });

        std::map<string, XGTDedicated::cpu_info_t> cpus {
            {"xgk", XGTDedicated::cpu_info_t::XGK},
            {"xgb_mk", XGTDedicated::cpu_info_t::XGB_MK},
            {"xgi", XGTDedicated::cpu_info_t::XGI},
            {"xgb_iec", XGTDedicated::cpu_info_t::XGB_IEC},
            {"xgr", XGTDedicated::cpu_info_t::XGR}
        };

        XGTDedicated::cpu_info_t _info { XGTDedicated::cpu_info_t::XGI };
        for(std::map<string, XGTDedicated::cpu_info_t>::iterator itr=cpus.begin(); itr!=cpus.end(); ++itr){
            if(!param.compare(itr->first)){
                _info = itr->second;
                break;
            }
        }

        return _info;
    }

    XGTDedicated::fenet_slot_t XGTDedicated::_set_slotIndex(const int& index){
        switch(index){
            case 0: return XGTDedicated::fenet_slot_t::SLOT0; break;
            case 1: return XGTDedicated::fenet_slot_t::SLOT1; break;
            case 2: return XGTDedicated::fenet_slot_t::SLOT2; break;
            case 3: return XGTDedicated::fenet_slot_t::SLOT3; break;
            case 4: return XGTDedicated::fenet_slot_t::SLOT4; break;
            case 5: return XGTDedicated::fenet_slot_t::SLOT5; break;
            case 6: return XGTDedicated::fenet_slot_t::SLOT6; break;
            case 7: return XGTDedicated::fenet_slot_t::SLOT7; break;
            case 8: return XGTDedicated::fenet_slot_t::SLOT8; break;
            case 9: return XGTDedicated::fenet_slot_t::SLOT9; break;
            case 10: return XGTDedicated::fenet_slot_t::SLOT10; break;
            case 11: return XGTDedicated::fenet_slot_t::SLOT11; break;
            case 12: return XGTDedicated::fenet_slot_t::SLOT12; break;
            case 13: return XGTDedicated::fenet_slot_t::SLOT13; break;
            case 14: return XGTDedicated::fenet_slot_t::SLOT14; break;
            case 15: return XGTDedicated::fenet_slot_t::SLOT15; break;
            default:
                return XGTDedicated::fenet_slot_t::SLOT0;
        }
    }

    XGTDedicated::fenet_base_t XGTDedicated::_set_baseIndex(const int& index){
        switch(index){
            case 0: return XGTDedicated::fenet_base_t::BASE0; break;
            case 1: return XGTDedicated::fenet_base_t::BASE1; break;
            case 2: return XGTDedicated::fenet_base_t::BASE2; break;
            case 3: return XGTDedicated::fenet_base_t::BASE3; break;
            case 4: return XGTDedicated::fenet_base_t::BASE4; break;
            case 5: return XGTDedicated::fenet_base_t::BASE5; break;
            case 6: return XGTDedicated::fenet_base_t::BASE6; break;
            case 7: return XGTDedicated::fenet_base_t::BASE7; break;
            case 8: return XGTDedicated::fenet_base_t::BASE8; break;
            case 9: return XGTDedicated::fenet_base_t::BASE9; break;
            case 10: return XGTDedicated::fenet_base_t::BASE10; break;
            case 11: return XGTDedicated::fenet_base_t::BASE11; break;
            case 12: return XGTDedicated::fenet_base_t::BASE12; break;
            case 13: return XGTDedicated::fenet_base_t::BASE13; break;
            case 14: return XGTDedicated::fenet_base_t::BASE14; break;
            case 15: return XGTDedicated::fenet_base_t::BASE15; break;
            default:
                return XGTDedicated::fenet_base_t::BASE0;
        }

    }

}