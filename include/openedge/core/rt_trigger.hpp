

/**
 * @file    rt_trigger.hpp
 * @brief   realtime signal trigger using POSIX timer (under 1Hz)
 * @author  Byugnhun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_INCLUDE_CORE_RT_TRIGGER_HPP_
#define _OPENEDGE_INCLUDE_CORE_RT_TRUGGER_HPP_

#include <time.h>
#include <signal.h>
#include <functional>

using namespace std;

namespace oe {
    namespace core {

        //The SIGRTMAX signals are intended to be used for user-defined purposes. They are real-time signals.
        #define SIGTIMER (SIGRTMAX)


        class rt_trigger {

            typedef void (*callback_trigger_ptr)(int);  //rt_trigger callback function

            public:
                rt_trigger(unsigned int hz, callback_trigger_ptr func);
                ~rt_trigger();

                const unsigned int get_freq() const { return (const unsigned int)_trigger_freq_hz; }
                void run();

            private:
                void _callback_func(int signo, siginfo_t* info, void* context);
                void _create_rt_timer(unsigned int hz);
                void _set_timer(int signo, unsigned int hz);


            private:
                timer_t _timer_id;
                std::function<void(int)> _ext_callback;

                unsigned int _trigger_freq_hz = 0;
                struct sigaction _sigact;
        };

    } //namespace core
}//namespace oe


#endif