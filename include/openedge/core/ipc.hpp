/**
 * @file    ipc.hpp
 * @author  Byunghun hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_IPC_HPP_
#define _OPENEDGE_CORE_IPC_HPP_

namespace oe {
    namespace core::task{ class driver; }
    namespace core::ipc {

        /* connectable */
        class connectable {

            friend class oe::core::task::driver;

            public:
                virtual void on_request() = 0;
                virtual void on_event() = 0;

            protected:


            
        };

    } /* namespace core::ipc */
} /* namespace oe */


#endif