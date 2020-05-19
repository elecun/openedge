
/**
 * @file    bus.hpp
 * @brief   LSIS PLC Bus
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

namespace oe {
    namespace plc {
        namespace lsis {
            
            class bus {
                public:
                bus(){

                }
                ~bus();

                void read(string address, unsigned short len);

                private:
                    int word_length = 1;
                    //byteTransform
            };

        } //namespace lsis
    } //namespace plc
} //namespace oe