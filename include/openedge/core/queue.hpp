

/**
 * @file    queue.hpp
 * @brief   queue container
 * @author  Byunghun Hwang
 */

#ifndef _OPENEDGE_CORE_QUEUE_HPP_
#define _OPENEDGE_CORE_QUEUE_HPP_

#include <queue>

using namespace std;

namespace oe {
    namespace core {

        template<class _t>
        class queue{
            public:

                void push(_t& cls) { _container.push(cls); }

            private:
                std::queue<_t> _container;

        };

    } //namespace core
} //namespace oe

#endif