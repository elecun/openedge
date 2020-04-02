
/**
 * @file    timer.hpp
 * @brief   Timer Engine
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_TIMER_HPP_
#define _OPENEDGE_CORE_TIMER_HPP_

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

using namespace std;

namespace oe {
    namespace core {

        typedef enum timer_type_t : unsigned int {
            PERIODIC = 0,
        } timer_type;

        class rt_timer {
            public:
            rt_timer();
            virtual ~rt_timer();
            
            virtual void start(long nsec, timer_type type = timer_type::PERIODIC);

            protected:
            virtual void call() = 0;

            private:
            timer_t _timer_id = 0;
            struct sigevent _seg_evt;
            struct sigaction _seg_act;
            struct itimerspec _time_spec;

            static void handler(int signo, siginfo_t* info, void* context) {
                (reinterpret_cast<rt_timer*>(info->si_value.sival_ptr))->call();
            }
        }

    } //namespace core
} //namespace oe

#endif