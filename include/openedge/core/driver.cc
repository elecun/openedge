

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
            if(load(_taskname.c_str())){
                profile reader;
                string path = "./"+_taskname+".json"; //same dir
                reader.load(path.c_str(), _profile);

                //show task configuration
                spdlog::info("* Task Profile <{}>", _taskname);
                spdlog::info(" + ({}) Cycle Time : {} nsec", _taskname, _profile.cycle_ns);
                spdlog::info(" + ({}) Version : {}", _taskname, _profile.version);
                spdlog::info(" + ({}) CPU Affinity : {}", _taskname, _profile.affinity);

                return _task_impl->configure();
            }
            return false;
        }

        void task_driver::execute(){
            if(_task_impl) {
                set_rt_timer(_profile.cycle_ns);
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
            string path = "./"+string(taskname); //same dir
            _task_handle = dlopen(path.c_str(), RTLD_LAZY|RTLD_LOCAL);
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
        void task_driver::set_rt_timer(unsigned long long nsec){
            
            /* Set and enable alarm */ 
            _sig_evt.sigev_notify = SIGEV_SIGNAL; 
            _sig_evt.sigev_signo = SIG_RUNTIME_TRIGGER; 
            _sig_evt.sigev_value.sival_ptr = _timer_id; 
            if(timer_create(CLOCK_REALTIME, &_sig_evt, &_timer_id)==-1)
                spdlog::error("timer create error");
        
            const unsigned long long nano = (1000000000L);
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
                    auto t_elapsed = std::chrono::high_resolution_clock::now();
                    spdlog::info("<{}>Processing Time : {} ns / {} ns",
                    _taskname,
                    std::chrono::duration<double, std::chrono::seconds::period>(t_elapsed - t_now).count(),
                    std::chrono::duration<double, std::chrono::seconds::period>(t_now - t_prev).count());
                    t_prev = t_now;
                }
                
            }
        }

    } //namespace core
} //namespace oe