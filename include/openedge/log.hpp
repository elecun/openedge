
/**
 * @file    log.hpp
 * @brief   class for logging on console or file \
 *          it is based on spdlog library (https://github.com/gabime/spdlog.git)
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_LOG_HPP_
#define _OPENEDGE_LOG_HPP_


namespace oe {

    class console {
        public:
            console();
            virtual ~console();

            void set_log_stream(const char* stream);

            void info(const char* logmsg);
            void warn(const char* logmsg);
            void critical(const char* logmsg);
            void error(const char* logmsg);
            void debug(const char* logmsg);
    }


} //namespace oe


#endif