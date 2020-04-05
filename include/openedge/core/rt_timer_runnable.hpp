

#ifndef _OPENEDGE_CORE_RT_TIMER_RUNNABLE_HPP_
#define _OPENEDGE_CORE_RT_TIMER_RUNNABLE_HPP_

#include "rt_timer.hpp"

namespace oe {
    namespace core {
        class rt_timer_callback : public rt_timer {
            public:
                class runnable {
                    public:
                        virtual void run() = 0;
                };

                void regist_runnable(runnable& h){
                    _runnable = &h;
                }

                void release_runnable() {
                    _runnable = nullptr;
                }

                void call() {
                    if(_runnable)
                        _runnable->run();
                }
            private:
                runnable* _runnable = nullptr;
        };
    } //namespace core
} //namespace oe

#endif