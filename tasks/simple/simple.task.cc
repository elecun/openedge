

#include "simple.task.hpp"
#include <external/spdlog/spdlog.h>

namespace oe {
    namespace task {

        bool simpletask::configure(){
            spdlog::info("simple.task configure()");
            return true;
        }

        void simpletask::execute(){
            spdlog::info("simple.task execute()");

        }

        void simpletask::cleanup(){
            spdlog::info("simple.task cleanup()");
        }

    } //namespace task
} //namespace oe