

#ifndef _OEWARE_CONTAINER_HPP_
#define _OEWARE_CONTAINER_HPP_

#include <queue>

using namespace std;

namespace oe {
    namespace abstract { class task; }
    namespace task {
        class container {
            public:
                container();
                ~container();

            unsigned int add(const char* taskname, oe::abstract::task* pInstance) {
                return _taskContainer.size();
            }

            private:
                std::queue<oe::abstract::task*> _taskContainer;

        };
    } //namespace task
} //namespace oe

#endif