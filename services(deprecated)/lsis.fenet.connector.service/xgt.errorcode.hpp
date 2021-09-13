
#ifndef _SERVICE_LSIS_FENET_XGT_PROTOCOL_ERROR_CODE_HPP_
#define _SERVICE_LSIS_FENET_XGT_PROTOCOL_ERROR_CODE_HPP_

#include <stdint.h>

namespace oe::bus::protocol {

    enum class xgt_errorcode_t : uint16_t {
        NORMAL_OPERATION = 0x0000,
        MODBUS_RECEIVE_FRAME_ERROR = 0x0001,        //모드버스 수신프레임 에러 (채널 초과)
        MODBUS_SIZE_ERROR = 0x0002,                 //모드버스 사이즈 에러
        DEVICE_AREA_REQUEST_ERROR = 0x0003,         //dedicated communication is establishing a connection with the server
        EXCEEDED_DEVICE_AREA_REQUEST_ERROR = 0x0004,//device area requested by the dedicated communication has been exceeded.
        TIMEOUT_ERROR = 0x0005,                     //타임아웃 에러
        DISCONNECT_NOTIFICATION_ERROR = 0x0006,     //disconnect 에러
        UNSUPPORTED_DATATYPE_ERROR = 0x0007,        //Server Service data type error
        ADDRESSTYPE_ERROR = 0x0008,                 //dedicated server - address type error
        XGT_COMMUNICATION_ERROR = 0x0010,           //P2P-XGT server communication or download error
        DATATYPE_ERROR = 0x0011,                    //p2p communication error
        FENET_ADDRESS_FORMAT_ERROR = 0x0012,        //error in the address format fo the FEnet dedicated communication request messsage.
        PARAMETER_STRUCTURE_ERROR = 0x0013,         //parameter 구조 에러
        P2P_EVENT_TRANFER_ERROR = 0x0015,           //p2p event 송신 에러
        P2P_EVENT_RECEIVE_ERROR = 0x0016,           //p2p event 수신 에러
        P2P_CONNECTION_ERROR = 0x0017,              //p2p 연결 에러
        CHANNEL_CONNECTION_OVERFLOW = 0x0051,       //채널 연결 오버플로우
        CHANNEL_SETTING_ERROR = 0x0062,             //채널 설정 에러
        INCORRECT_COMPANYID_ERROR = 0x0075,         //company id is set incorrectly in the request message header for FEnet.
        INCORRECT_SIZE_ERROR = 0x0076,              //size of the request message header for FEnet dedicated comm. is incorrect
        INCORRECT_CHECKSUM_ERROR = 0x0077,          //checksum in the request message header of FEnet dedicated comm. is set incorrectly
        INCORRECT_COMMAND_ERROR = 0x0078,
        INTERNAL_BUFFER_ALLOCATION_ERROR = 0x00f0,  //내부 버퍼 할당 오류
        USER_DEFINED_DATA_TRANSFER_ERROR = 0xff00,  //사용자정의 데이터 전송 오류(사용자 정의)
        USER_DEFINED_EVENT_RECEIVE_ERROR = 0xff01,  //사용자정의 event 수신 에러
        INTERNAL_DATA_TRANSFER_OVERFLOW = 0xffd7,   //내부 데이터 송신 오버플로우
        INTERNAL_INTERLOCK_ERROR = 0xffeb,          //내부 인터록 오류
        TRANSFER_FRAME_SIZE_ERROR = 0xffff,         //server 설정으로 connection 요청시, 사용자정으에서 송신 프레임 사이즈 에러
    };

    enum class xgt_email_errorcode_t : uint16_t {
        EMAIL_ADDRESS_ERROR = 0x0001,                       // email 어드레스 오류
        EMAIL_FILE_ERROR = 0x0002,                          //email 파일 오류
        EMAIL_SERVER_LOGON_ERROR = 0x0003,                  //email 서버 로그온 오류
        EMAIL_TRANSFER_ERROR = 0x0005,                      //email 송신 오류
        EMAIL_DISABLE = 0x0007,                             //email disable
        EMAIL_DOWNLOAD_ERROR = 0x5d52,                      //email 다운로드 에러
        INTERNAL_TRANSFER_BUFFER_TIMEOUT_ERROR = 0xffce,    //내부 송신 버퍼 타임아웃 오류
        INTERNAL_TRANSFER_BUFFER_RESET_ERROR = 0xffd6,      //내부 송신 버퍼 리셋 오류
        INTERNAL_TRANSFER_BUFFER_REMOVE_ERROR = 0xffd9,     //내부 송신 버퍼 삭제 오류
        INTERNAL_TRANSFER_BUFFER_SIZE_ERROR = 0xffea,       //내부 송신 버퍼 사이즈 오류
        INTERNAL_TRANSFER_BUFFER_INTERLOCK_ERROR = 0xffe8,  //내부 송신 버퍼 인터락 오류
        INTERNAL_TRANSFER_BUFFER_POINTER_ERRPR = 0xfff1,    //내부 송신 버퍼 포인터 오류
        INTERNAL_TRANSFER_BUFFER_ERROR = 0xfff2             //내부 송신 버퍼 오류
    };

    enum class xgt_comm_errorcode_t : uint16_t {
        BLOCK_COUNT_ERROR = 0x0001,             //개별 읽기/쓰기 요청시 블록 수가 16보다 큼
        INVALID_DATATYPE_ERROR = 0x0002,        //X,B,W,D,L이 아닌 데이터 타입을 수신했음
        UNSUPPORTED_DEVICE_REQUEST_ERROR = 0x0003,  //서비스 되지 않는 디바이스를 요구한 경우(XGK:P,M,L,K,R..., XGI:I,Q,M,...)
        INVALID_ADDRESS_REQUEST_ERROR = 0x0004,     //각 디바이스별 지원하는 영역을 초과해서 요구한 경우
        OVER_SINGLE_BLOCKSIZE_REQUEST_ERROR = 0x0005,      //한번에 최대 1400byte까지 읽거나 쓸 수 있는데 초과해서 요청한 경우 (개별 블록 사이즈)
        OVER_BLOCKSIZE_REQUEST_ERROR = 0x0006,          //한번에 최대 1400byte까지 읽거나 슬 수 있는데 초과해서 요청한 경우 (블록별 총 사이즈)
        INVALID_HEADER_IDENTITY_ERROR = 0x0075,           //전용 서비스에서 프레임 헤더의 선두 부분이 잘못된 경우(LSIS-GLOFA)
        HEADER_SIZE_ERROR = 0x0076,               //전용 서비스에서 프레임 헤더의 length가 잘못된 경우
        HEADER_CHECKSUM_ERROR = 0x0077,             //전용 서비스에서 프레임 헤더의 checksum이 잘못된 경우
        HEADER_COMMAND_ERROR = 0x0078               //전용 서비스에서 명령어가 잘못된 경우
    };

} //namespace 


#endif