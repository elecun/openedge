
/**
 * @file    cpu.info.hpp
 * @brief   LSIS PLC CPU Info Header
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

namespace oe {
    namespace plc {
        namespace lsis {

            typedef enum class CpuInfo : int {
                XGK = 1,
                XGI,
                XGR,
                XGB_MK,
                XGB_IEC,
            };

            typedef enum class CpuStatus : int {
                RUN = 1,
                STOP,
                ERROR,
                DEBUG
            };

            typedef enum class FlagBit : int {
                Flag1 = 1,
                Flag2 = 2,
                Flag4 = 4,
                Flag8 = 8,
                Flag16 = 16,
                Flag32 = 32,
                Flag64 = 64,
                Flag128 = 128,
                Flag256 = 256,
            };

        } //namespace lsis
    } //namespace plc
} //namespace oe