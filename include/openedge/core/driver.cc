

#include "driver.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <dlfcn.h>
#include <signal.h>
#include <chrono>

#define SIG_RUNTIME_TRIGGER (SIGRTMIN)

namespace oe {
    namespace core {

        task_driver::task_driver(const char* taskname):_taskname(taskname){
        }

        task_driver::~task_driver(){
            unload();
        }


        bool task_driver::configure(){
            spdlog::info("Configured task driver for {}", _taskname);
            if(load(_taskname.c_str())){
                profile reader;
                string path = "./"+_taskname+".json"; //same dir
                reader.load(path.c_str(), _profile);

                return _task_impl->configure();
            }
            return false;
        }

        void task_driver::execute(){
            spdlog::info("Execute task driver");
            if(_task_impl) {
                set_rt_timer(10*1000*1000); //driver call via rt timer signal
                _ptr_thread = new thread{ &oe::core::task_driver::do_process, this };
            }
        }

        void task_driver::cleanup(){
            if(_task_impl)
                _task_impl->cleanup();
            unload();
        }

        //load task component
        bool task_driver::load(const char* taskname){
            spdlog::info("Loading Task : {}", taskname);

            string path = "./"+string(taskname); //same dir
            _task_handle = dlopen(path.c_str(), RTLD_LAZY|RTLD_GLOBAL);
            if(_task_handle){
                create_rt_task pfcreate = (create_rt_task)dlsym(_task_handle, "create");
                if(!pfcreate){
                    spdlog::error("{} cannot be loaded : {}", taskname);
                    dlclose(_task_handle);
                    _task_handle = nullptr;
                    return false;
                }
                _task_impl = pfcreate();
                return true;
            }
            else{
                spdlog::error("{} cannot be loaded : {}", taskname);
            }
            return false;
        }

        //unload task component
        void task_driver::unload(){
            if(_task_impl){
                spdlog::info("Releasing Task : {}", _taskname);
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

        //all component has own rt timer
        void task_driver::set_rt_timer(long nsec){
            
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

        //concreate process impl.
        void task_driver::do_process() {

            //signal set for threading
            sigset_t thread_sigmask;
            sigemptyset(&thread_sigmask);
            sigaddset(&thread_sigmask, SIG_RUNTIME_TRIGGER);
            sigaddset(&thread_sigmask, SIGTERM);
            int _sig_no;
            auto t_prev = std::chrono::high_resolution_clock::now();

            while(1){
                sigwait(&thread_sigmask, &_sig_no);
                if(_sig_no==SIG_RUNTIME_TRIGGER){
                    auto t_now = std::chrono::high_resolution_clock::now();
                    if(_task_impl){
                        _task_impl->execute();
                    }
                    spdlog::info("Trigger Time : {}", std::chrono::duration<double, std::chrono::seconds::period>(t_now - t_prev).count());
                    t_prev = t_now;
                    //spdlog::info("Signal Duration : {}",t_now);
                }
                
            }
        }

    } //namespace core
} //namespace oe