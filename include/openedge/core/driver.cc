

#include "driver.hpp"
#include <external/spdlog/spdlog.h>
#include <dlfcn.h>
#include <signal.h>

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

        void task_driver::init_timer(){
            //create handler which catches the signal SIG
            _sig_act.sa_flags = SA_SIGINFO;
            _sig_act.sa_sigaction = timer_handler;
            sigisemptyset(&_sig_act.sa_mask);
            if(sigaction(SIG_RUNTIME_TRIGGER, &_sig_act, nullptr)==-1)
                spdlog::error("Could not create signal handler");

            //create timer
            _sig_evt.sigev_notify = SIGEV_SIGNAL;
            _sig_evt.sigev_signo = SIG_RUNTIME_TRIGGER;
            _sig_evt.sigev_value.sival_ptr = this;
            if(timer_create(CLOCK_REALTIME, &_sig_evt, &_timer_id)==-1)
                spdlog::error("Could not create timer");

            long nsec = 100*1000*1000;
            timer_type type = timer_type::PERIODIC;

            switch(type){
            case timer_type::PERIODIC:
                const long nano = (1000000000L);
                _time_spec.it_value.tv_sec = nsec / nano;
                _time_spec.it_value.tv_nsec = nsec % nano;
                _time_spec.it_interval.tv_sec = nsec / nano;
                _time_spec.it_interval.tv_nsec = nsec % nano;
            break;
        }

        if(timer_settime(_timer_id, 0, &_time_spec, nullptr)==-1)
            spdlog::error("timer setting failed");

        }

        void task_driver::stop_timer(){
            struct itimerspec _new_time_spec;
            _new_time_spec.it_value.tv_sec = 0;
            _new_time_spec.it_value.tv_nsec = 0;
            _new_time_spec.it_interval.tv_sec = 0;
            _new_time_spec.it_interval.tv_nsec = 0;
            timer_settime(_timer_id, 0, &_new_time_spec, &_time_spec);
            timer_delete(_timer_id);
            signal(SIG_RUNTIME_TRIGGER, SIG_IGN);
        }

        void task_driver::execute(){
            spdlog::info("Execute task driver for {}", _taskname);

            if(_task_impl){
                _ptr_thread = new thread{ &oe::core::task_driver::proc, this };
            }

            init_timer();
        }

        void task_driver::cleanup(){
            spdlog::info("Cleanup task driver for {}", _taskname);

            stop_timer();

            if(_task_impl)
                _task_impl->cleanup();
            unload();
        }

        //load task
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

        void task_driver::call(){
            _cv.notify_one();
        }

        //execute in thread
        void task_driver::proc() {
            if(_task_impl){
                while(1){
                    std::unique_lock<std::mutex> lock(_mutex);
                    _cv.wait(lock);
                    _task_impl->execute();
                    
                }
            }
        }

    } //namespace core
} //namespace oe