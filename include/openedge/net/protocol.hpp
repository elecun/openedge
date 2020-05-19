
/**
 * @file    protocol.hpp
 * @brief   Network Protocol Interface
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */


#ifndef _OPENEDGE_NET_PROTOCOL_HPP_
#define _OPENEDGE_NET_PROTOCOL_HPP_

#include <vector>
#include <cstdint>

namespace oe {
    namespace net {

        class protocolInterface {
            public:
                virtual int getHeaderSize() const = 0;  //getting header length
                virtual int getContentSize() const = 0; //getting content length by header
                
                virtual int getInvokeId() = 0;  //ID from header
                
                virtual std::vector<uint8_t> getHeader() const = 0; //getting header data
                virtual void setHeader(const std::vector<uint8_t>& data) = 0; //setting header data

                virtual std::vector<uint8_t> getContent() const = 0; //getting body data
                virtual void setContent(const std::vector<uint8_t>& data) = 0; //setting body data

        }; //class
        
    } //namespace net
 }//namespace oe

#endif