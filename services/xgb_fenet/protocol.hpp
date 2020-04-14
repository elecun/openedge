
/**
 * @file    LSIS_FEnet.hpp
 * @brief   LSIS XGT Fast Ethernet dedicated Protocol
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */


#ifndef _OPENEDGE_NET_PROTOCOL_LSIS_FENET_HPP_
#define _OPENEDGE_NET_PROTOCOL_LSIS_FENET_HPP_

#include <openedge/net/protocol.hpp>
#include <vector>
#include <cstring>

using namespace std;

namespace oe {
    namespace net {
        namespace protocol {

            #define DEFAULT_HEADER_LEN  20

            typedef struct fastEthernetHeader {
                uint8_t     companyId[10] = {0, };
                uint16_t    plcInfo;
                uint8_t     cpuInfo;
                uint8_t     sof;    //source of frame(->plc:0x33, <-plc:0x11)
                uint16_t    invokeId;
                uint16_t    length;
                uint8_t     position;
                uint8_t     reserved;
                bool isValid(){
                    const uint8_t lsis[] = {'L','S','I','S','-','X','G','T'}; //2 bytes are reserved.
                    int idx = 0;
                    bool rc = true;
                    for(const uint8_t& c:lsis)
                        if(companyId[idx++]!=c) {
                            rc = false; break;
                        }
                    return rc;
                }
                fastEthernetHeader& operator=(const fastEthernetHeader& other){
                    memcpy(this, &other, sizeof(this));
                }
            };

            //Class Impl.
            class fastEthernetProtocol : public oe::net::protocolInterface {
            public:

                int getHeaderSize() const override {
                    _header.size();
                }

                int getContentSize() const override {
                    return (int)(_header[16] << 8 | _header[17]);
                }

                int getInvokeId() override {
                    return (_header[14] << 8 | _header[15]);
                }
                
                std::vector<uint8_t> getHeader() const override {
                    return std::vector<uint8_t>(_header);
                }
                void setHeader(const std::vector<uint8_t>& data) override {
                    _header = data;
                }

                std::vector<uint8_t> getContent() const override {
                    return _content;
                }

                void setContent(const std::vector<uint8_t>& data) override {
                    _content = data;
                }


            private:
                std::vector<uint8_t> _header;
                std::vector<uint8_t> _content;
                std::vector<uint8_t> _send;

            }; //class

        }

    } //namespace net
 }//namespace oe

#endif