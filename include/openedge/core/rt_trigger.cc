

#include "rt_trigger.hpp"
#include "excode.hpp"

namespace oe {
    namespace core {

        rt_trigger::rt_trigger(unsigned int hz, callback_trigger_ptr func)
        :_trigger_freq_hz(hz), _ext_callback(func) {
            if(_ext_callback)
                _create_rt_timer(hz);
        }

        rt_trigger::~rt_trigger(){
            timer_delete(_timer_id);
        }

        void rt_trigger::_callback_func(int signo, siginfo_t* info, void* context){
            if(signo==SIGTIMER)
                _ext_callback(signo);
        }

        void rt_trigger::_create_rt_timer(unsigned int hz){

            sigemptyset(&_sigact.sa_mask);
            _sigact.sa_flags = SA_SIGINFO;
            _sigact.sa_sigaction = (void*)(this->_callback_func);

            if(sigaction(SIGTIMER, &_sigact, nullptr) == -1) {
                throw excode::OE_SIGACTION_FAIL;
            }

            _set_timer(SIGTIMER, hz);
        }

        void rt_trigger::_set_timer(int signo, unsigned int hz){
            struct sigevent sigev;
            timer_t timerid;
            struct itimerspec itval;
            struct itimerspec oitval;

            // Create the POSIX timer to generate signo
            sigev.sigev_notify = SIGEV_SIGNAL;
            sigev.sigev_signo = signo;
            sigev.sigev_value.sival_ptr = &timerid;

            if(timer_create(CLOCK_REALTIME, &sigev, &timerid)==0) {
                itval.it_value.tv_sec = 0;
                itval.it_value.tv_nsec = (long)(1/_trigger_freq_hz*1e9);
                itval.it_interval.tv_sec = itval.it_value.tv_sec;
                itval.it_interval.tv_nsec = itval.it_value.tv_nsec;

                if(timer_settime(timerid, 0, &itval, &oitval)!=0) {
                    throw excode::OE_SETTIME_FAIL;
                }
            }
            else {
                throw excode::OE_TIMER_CREATE_FAIL;
            }
        }


    } //namespace core
} //namespace oe