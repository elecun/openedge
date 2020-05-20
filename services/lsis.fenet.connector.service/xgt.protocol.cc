

#include "xgt.protocol.hpp"
#include <3rdparty/spdlog/spdlog.h>

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

        _set_default_header(tmpPacket, cpu_info_t::XGI, sof_t::CLIENT, fenet_slot_t::SLOT0, fenet_base_t::BASE0);
        _set_default_body(tmpPacket, static_cast<uint16_t>(command_code_t::READ_REQUEST), static_cast<uint16_t>(datatype_t::BLOCK), 0x0001, address, (uint16_t)count);
        _packet_refine(tmpPacket);
    
        return tmpPacket;
    }

    uint8_t XGTDedicated::chksum(vector<uint8_t>& packet){ //checksum8 modulo 256 (sum of bytes % 256)
        uint8_t sum = 0x00;
        for(int i=0;i<HEADER_SIZE;i++)
            sum += packet[i];
        return (sum&0xff);
    }

    void XGTDedicated::setParameters(string config){
        try {
            this->proto_configs = json::parse(config);                    
            spdlog::info("Protocol Parameters : {}", proto_configs.dump());
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

}