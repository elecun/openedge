
/**
 * @file    log.cpp
 * @brief   common log class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#include <3rdparty/spdlog/spdlog.h>
#include <3rdparty/spdlog/sinks/stdout_color_sinks.h>

using namespace std;

namespace oe {
    namespace log {

class console {
    public:
        console();
        virtual ~console();

        template<typename... Args>
        void info(const char* logmsg, Args... args) {

        }

        private:
            std::shared_ptr<spdlog::logger> 
}


    } //namespace log
} //namespace oe