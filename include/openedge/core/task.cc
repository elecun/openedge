
#include "task.hpp"

namespace oe {
    namespace core {

        abstractTask::abstractTask(const char* taskname)
        :_taskname(taskname)
        ,_working(false) {

        }

        abstractTask::~abstractTask() {

        }

        void abstractTask::start(){
            _working = true;
        }

        void abstractTask::stop(){
            _working = false;
        }



    } //namespace core
} //namespace oe