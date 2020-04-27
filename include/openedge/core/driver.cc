

#include "driver.hpp"
#include <3rdparty/spdlog/spdlog.h>
#include <dlfcn.h>
#include <signal.h>
#include <chrono>
#include <openedge/core/profile.hpp>
#include <openedge/util/validation.hpp>

#define SIG_RUNTIME_TRIGGER (SIGRTMIN)

namespace oe::core::task {

    driver::driver(const char* taskname){
        try {
            if(load(taskname)){
                if(_taskImpl){
                    string path = "./"+string(taskname)+".json"; //same dir
                    if(exist(path.c_str()))
                        _taskImpl->_profile = new oe::core::profile(path.c_str()); //load profile
                    else
                        spdlog::error("<{}> profile does not exist", taskname);
                    _taskImpl->_taskname = taskname;
                    _taskImpl->setStatus(oe::core::task::runnable::TASK_STATUS::STOPPED);
                }
            }
        }
        catch(std::runtime_error& e){
            spdlog::error("{} driver cannot load", taskname);
        }
    }

    driver::~driver(){
        if(_taskImpl){
            if(_taskImpl->_profile){
                delete _taskImpl->_profile;
                _taskImpl->_profile = nullptr;
            }   
        }

        unload();
    }


    bool driver::configure(){
        if(_taskImpl){
            return _taskImpl->configure();
        }
        return false;
    }

    void driver::execute(){
        if(_taskImpl) {
            if(_taskImpl->_profile){
                unsigned long long rtime = _taskImpl->_profile->data["info"]["cycle_ns"].get<unsigned long long>();
                spdlog::info("<{}> RT Time Period : {}",_taskImpl->_taskname, rtime);
                set_rt_timer(rtime);
                _ptrThread = new thread{ &oe::core::task::driver::do_process, this };
            }
        }
    }

    void driver::cleanup(){
        if(_taskImpl)
            _taskImpl->cleanup();
        unload();
    }

    //load task component
    bool driver::load(const char* taskname){
        string path = "./"+string(taskname); //same dir
        _task_handle = dlopen(path.c_str(), RTLD_LAZY|RTLD_LOCAL);
        if(_task_handle){
            create_task pfcreate = (create_task)dlsym(_task_handle, "create");
            if(!pfcreate){
                spdlog::error("{} cannot be loaded : {}", taskname);
                dlclose(_task_handle);
                _task_handle = nullptr;
                return false;
            }
            _taskImpl = pfcreate();
            return true;
        }
        else{
            spdlog::error("{} cannot be loaded : {}", taskname);
        }
        return false;
    }

    //unload task component
    void driver::unload(){
        if(_taskImpl){
            release_task pfrelease = (release_task)dlsym(_task_handle, "release");
            if(pfrelease)
                pfrelease();
            
            _taskImpl = nullptr;
        }
        if(_task_handle){
            dlclose(_task_handle);
            _task_handle = nullptr;
        }

    }

    //all component has own rt timer
    void driver::set_rt_timer(unsigned long long nsec){
        
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
    void driver::do_process() {

        //signal set for threading
        sigset_t thread_sigmask;
        sigemptyset(&thread_sigmask);
        sigaddset(&thread_sigmask, SIG_RUNTIME_TRIGGER);
        sigaddset(&thread_sigmask, SIGTERM);
        int _sig_no;
        auto t_prev = std::chrono::high_resolution_clock::now();
        _taskImpl->_status = oe::core::task::runnable::TASK_STATUS::RUNNING;

        while(1){
            sigwait(&thread_sigmask, &_sig_no);
            if(_sig_no==SIG_RUNTIME_TRIGGER){
                auto t_now = std::chrono::high_resolution_clock::now();
                if(_taskImpl){
                    _taskImpl->execute();
                }
                auto t_elapsed = std::chrono::high_resolution_clock::now();
                // spdlog::info("<{}>Processing Time : {} ns / {} ns",
                // _taskImpl->_taskname,
                // std::chrono::duration<double, std::chrono::seconds::period>(t_elapsed - t_now).count(),
                // std::chrono::duration<double, std::chrono::seconds::period>(t_now - t_prev).count());
                // t_prev = t_now;
            }
            
        }
    }
} //namespace oe::core::task