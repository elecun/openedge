
/**
 * @file    xgb_fenet.service.hpp
 * @brief   LSIS XGB FEnet
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#include <openedge/net/netbase.hpp>
#include "xgb_fenet.hpp"


namespace oe {
    namespace PLC {
        namespace LSIS {

            class fenetConnectorService : public net::netBase {
                public:
                fenetConnectorService();
                ~fenetConnectorService();

                bool connect(const char* ipv4_address, int port);

                private:
                    net::XGBFEnet* _fenet = nullptr;
            };

        } //namespace LSIS
    } //namesoace plc
} //namesoace oe