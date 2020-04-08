

#include "driver.hpp"
#include <external/spdlog/spdlog.h>
#include <dlfcn.h>
#include <signal.h>

#define SIG_RUNTIME_TRIGGER (SIGRTMIN)

namespace oe {
    namespace core {

        task_driver::task_driver(const char* taskname):_taskname(taskname){
            spdlog::info("Created task driver for {}", taskname);
        }

        task_driver::~task_driver(){
            spdlog::info("Terminated task driver for {}", _taskname);
            unload();
        }


        bool task_driver::configure(){
            spdlog::info("Configured task driver for {}", _taskname);
            if(load(_taskname.c_str())){
                return _task_impl->configure();
            }
            return false;
        }

        void task_driver::execute(){
            spdlog::info("Execute task driver");
            if(_task_impl) {
                make_timer(10*1000*1000);
                _ptr_thread = new thread{ &oe::core::task_driver::proc, this }; //running on thread
            }
        }

        void task_driver::cleanup(){
            spdlog::info("Cleanup task driver for {}", _taskname);

            if(_task_impl)
                _task_impl->cleanup();
            unload();
        }

        //load task component
        bool task_driver::load(const char* taskname){
            string path = "./"+string(taskname); //same dir
            _task_handle = dlopen(path.c_str(), RTLD_LAZY|RTLD_GLOBAL);
            if(_task_handle){
                create_rt_task pfcreate = (create_rt_task)dlsym(_task_handle, "create");
                if(!pfcreate){
                    dlclose(_task_handle);
                    _task_handle = nullptr;
                    return false;
                }

                _task_impl = pfcreate();
                return true;
            }
            else{
                spdlog::error("{} cannot be loaded.", _taskname);
            }
            return false;
        }

        //unload task component
        void task_driver::unload(){
            if(_task_impl){
                release_rt_task pfrelease = (release_rt_task)dlsym(_task_handle, "release");
                if(pfrelease)
                    pfrelease();
                
                _task_impl = nullptr;
            }
            if(_task_handle){
                dlclose(_task_handle);
                _task_handle = nullptr;
            }

        }

        //make timer
        void task_driver::make_timer(long nsec){
        
            /* Set up signal handler. */ 
            _sig_act.sa_flags = SA_SIGINFO; 
            _sig_act.sa_sigaction = timer_handler; 
            sigemptyset(&_sig_act.sa_mask); 
            if(sigaction(SIG_RUNTIME_TRIGGER, &_sig_act, nullptr)==-1)
                spdlog::error("Sigaction error");
            
            /* Set and enable alarm */ 
            _sig_evt.sigev_notify = SIGEV_SIGNAL; 
            _sig_evt.sigev_signo = SIG_RUNTIME_TRIGGER; 
            _sig_evt.sigev_value.sival_ptr = _timer_id; 
            if(timer_create(CLOCK_REALTIME, &_sig_evt, &_timer_id)==-1)
                spdlog::error("timer create error");
        
            const long nano = (1000000000L);
            _time_spec.it_value.tv_sec = nsec / nano;
            _time_spec.it_value.tv_nsec = nsec % nano;
            _time_spec.it_interval.tv_sec = nsec / nano;
            _time_spec.it_interval.tv_nsec = nsec % nano;

            if(timer_settime(_timer_id, 0, &_time_spec, nullptr)==-1)
                spdlog::error("timer setting error");
        }

        void task_driver::call(int signo, siginfo_t* info, void* context){
            _ready = true;
            //raise(SIG_RUNTIME_TRIGGER);
            //spdlog::info("raise signal");
            

            //struct timespec x; 
            //clock_gettime(CLOCK_REALTIME,&x); 
            //spdlog::info("{}.{:09d}", (long)x.tv_sec, (long)x.tv_nsec);

            // spdlog::info("RTS queue : {}", info->si_code);
            // spdlog::info("User RTS signal : {}", info->si_pid);
            // spdlog::info("Sig Number : {}", info->si_signo);
            // spdlog::info("User Data is {}", info->si_value.sival_int);
            
            // if(signo==SIG_RUNTIME_TRIGGER){
            //     clock_gettime(CLOCK_REALTIME,&x); 
            //     spdlog::info("{}{}", (long)x.tv_sec, (long)x.tv_nsec);
            // }
        }

        void task_driver::sa_handler_usr(int nSigNum){
            printf("\t[%lu] Signal(%s)\n", 
			pthread_self(),
			nSigNum == SIGUSR1? "USR1":"USR2");
        }

        //concreate process impl.
        void task_driver::proc() {
            spdlog::info("Starting Thread ID : {}", (long)pthread_self());
            printf("Thread %d:\n", (int)pthread_self());

            sigset_t thread_sigmask;
            sigemptyset(&thread_sigmask);
            sigaddset(&thread_sigmask, SIG_RUNTIME_TRIGGER);
            sigaddset(&thread_sigmask, SIGTERM);
            int nSigNum;
	        int nErrno;

            while(1){
                nErrno = sigwait(&thread_sigmask, &nSigNum);
                if(nErrno > 0){
                    spdlog::error("sigwait error");
                }
                switch(nSigNum)
                {
                    case 34:
                        //sa_handler_usr(nSigNum);
                        spdlog::info("got signal");
                        break;
                    case SIGTERM:
                        printf("[signal SIGTERM]\n");
                        exit(EXIT_SUCCESS);
                    default:
                        break;
                }
            }
        }

    } //namespace core
} //namespace oe