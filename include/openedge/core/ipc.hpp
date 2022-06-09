/**
 * @file ipc.hpp
 * @author Byunghun Hwang *bh.hjwang@iae.re.kr)
 * @brief IPC using Shared Memory
 * @version 0.1
 * @date 2022-06-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_CORE_IPC_HPP_
#define _OPENEDGE_CORE_IPC_HPP_

#include <3rdparty/json.hpp>

using json = nlohmann::json;

namespace oe::core::ipc {

    class connector {
        public:
            connector() = default;
            ~connector() = default;

            const json read(const char* channel){
                
            }

    }; /* end class */
    
} /* end namespace */

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