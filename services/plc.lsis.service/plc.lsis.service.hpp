
/**
 * @file    plc.lsis.service.hpp
 * @brief   LSIS PLC Service
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_SERVICE_PLC_LSIS_HPP_
#define _OPENEDGE_SERVICE_PLC_LSIS_HPP_

#include <openedge/core/export.hpp>
#include <stdint.h>
#include <type_traits>

template <typename _Type>
    using SignedOnly = typename std::enable_if<std::is_signed<_Type>::value>::value* ;
template <typename _Type>
    using UnsignedOnly = typename std::enable_if<std::is_unsigned<_Type>::value>::value* ;


class EXPORTED plcLsisService {
    public:
        plcLsisService();
        ~plcLsisService();

        //common service interface
        bool open();
        void close();

        //support funtion
        uint8_t readByte(const char* address);

    private:

        template<typename _returnType, UnsignedOnly<_returnType> = nullptr>
        _returnType read(const char* address); //ex. read("%mw100")

        //void read_n(const char* variable, const unsigned int count);    //read n number of data


}; //class

//typedef plcLsisService*(*create_service)(void);
typedef void(*create_service)(void);
typedef void(*release_service)(void);

#ifdef  __cplusplus
extern "C" {
#endif

static plcLsisService* pService = nullptr;

EXPORTED void createService(void) { pService = new plcLsisService(); }
EXPORTED void releaseService(void) { if(pService) { delete pService; pService=nullptr; }}
EXPORTED uint8_t readByte(const char* address) { return pService->readByte(address); }

#ifdef  __cplusplus
}
#endif

#endif