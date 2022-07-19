/**
 * @file driver.cc
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Component Driver Handling
 * @version 0.1
 * @date 2022-06-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "driver.hpp"
#include <dlfcn.h>
#include <signal.h>
#include <chrono>
#include <openedge/core/profile.hpp>
#include <openedge/util/validation.hpp>
#include <stdexcept>
#include <openedge/core/registry.hpp>
#include <openedge/core/def.hpp>
#include <openedge/log.hpp>
#include <filesystem> //to use c++17 filesystem

namespace fs = std::filesystem;
using namespace std;
using namespace oe::core;


#if defined(linux) || defined(__linux) || defined(__linux__)
    static const int SIG_RUNTIME_TRIGGER = (SIGRTMIN);     //signal #64 : Runtime Error
    static const int SIG_PAUSE_TRIGGER = (SIGRTMIN-1);     //signal #63 : Pause Process
    static const int SIG_RESUME_TRIGGER = (SIGRTMIN-2);    //signal #62 : Resume Process
    static const int SIG_STOP_TRIGGER = (SIGRTMAX-3);      //signal #61 : Process Termination
#endif


namespace oe::core::task {

    /**
     * @brief Construct a new driver instance with its profile
     * 
     * @param taskname Task Name, It must be unique.
     */
    driver::driver(const char* taskname){
        try {
            if(load(taskname)){
                if(_taskImpl){
                    /* load profile */
                    if(registry->find(PATH_BIN_DIR)){
                        fs::path _bin = registry->get<string>(PATH_BIN_DIR);
                        fs::path _task = _bin / fs::path{fmt::format("{}{}",taskname, FILE_EXT_COMPONENT)};
                        fs::path _profile = _bin / fs::path{fmt::format("{}{}",taskname, FILE_EXT_PROFILE)};

                        if(fs::exists(_profile)){
                            _taskImpl->_profile = make_unique<core::profile>(_profile.c_str());
                            _taskImpl->_taskname = taskname;
                            _taskImpl->set_status(task::status_d::IDLE);
                        }
                        else {
                            console::error("<{}> profile does not exist.", taskname);
                        }
                    }
                }
                else {
                    console::error("Component successfully loaded, but the instance has null pointer.");
                }
            }
            else {
                _taskImpl = nullptr;
                console::error("Component load failed.");
            }
        }
        catch(std::runtime_error& e){
            console::error("<{}> driver cannot be loadded ({})", taskname, e.what());
        }
    }

    driver::driver(task::runnable* instance){
        try {
            _taskImpl = std::move(instance);
        }
        catch(std::runtime_error& e){
            if(instance){
                console::error("<{}> driver cannot be loadded ({})", instance->get_name(), e.what());
            }
            
        }
    }

    driver::driver(fs::path component){

    }

    driver::~driver(){
        unload();
    }


    bool driver::configure(){
        try {
            if(_taskImpl){
                if(_taskImpl->rtype==task::rtype_d::RT){
                    _taskImpl->_option.check_jitter = _taskImpl->get_profile()->data["info"]["policy"]["check_jitter"].get<bool>();
                    _taskImpl->_option.check_overrun = _taskImpl->get_profile()->data["info"]["policy"]["check_overrun"].get<bool>();
                }
                
                return _taskImpl->configure();
            }
        }
        catch(const std::runtime_error& e){
            console::error("Runtime Error : {}", e.what());
        }

        return false;
    }

    void driver::execute(){
        if(_taskImpl) {
            if(_taskImpl->get_status()==task::status_d::STOPPED || _taskImpl->get_status()==task::status_d::IDLE){
                if(_taskImpl->rtype==task::rtype_d::NT || _taskImpl->rtype==task::rtype_d::RT){
                    if(_taskImpl->_profile){
                        unsigned long long rtime = _taskImpl->_profile->data["info"]["cycle_ns"].get<unsigned long long>();
                        console::info("<{}> Time Period : {} ns",_taskImpl->get_name(), rtime);
                        set_rt_timer(rtime);
                        _ptrThread = new thread{ &oe::core::task::driver::do_process, this };
                    }
                }
                else {
                    console::info("<{}> task is not a runnable(periodic) task.", _taskImpl->get_name());
                }
            }
            else {
                console::warn("<{}> is still working. Task should be on STOPPED or IDLE state for execution.", _taskImpl->get_name());
            }
        }
        else {
            console::error("Invalid task instance. The instance is null.");
        }
    }

    void driver::cleanup(){
        timer_delete(_timer_id);    //delete timer
        console::info("Cleanup <{}>", _taskImpl->get_name());
        _taskImpl->set_status(task::status_d::STOPPED);
        if(_taskImpl)
            _taskImpl->cleanup();
        unload();
    }

    void driver::pause(){
        if(_taskImpl){
            console::warn("Not support yet.");
        }
    }

    void driver::resume(){
        if(_taskImpl){
            console::warn("Not support yet.");
        }
    }

    bool driver::load(const char* taskname){

        if(!taskname){
            console::warn("Task was not specified. It must be requred.", taskname);
            return false;
        }

        if(registry->find(PATH_BIN_DIR)){

            fs::path _bin = registry->get<string>(PATH_BIN_DIR);
            fs::path _task = _bin / fs::path{fmt::format("{}{}",taskname, FILE_EXT_COMPONENT)};
            fs::path _profile = _bin / fs::path{fmt::format("{}{}",taskname, FILE_EXT_PROFILE)};

            // 1. check file existance
            if(!fs::exists(_task)){
                console::error("{}{} doest not exist. please check path or task file.", taskname, FILE_EXT_COMPONENT);
                return false;
            }

            if(!fs::exists(_profile)){
                console::error("{}{} doest not exist. please check path or profile file.", taskname, FILE_EXT_PROFILE);
                return false;
            }
            
            // 2. open component(shared library)
            _task_handle = dlopen(_task.c_str(), RTLD_LAZY|RTLD_LOCAL);
            if(_task_handle){
                create_task pfcreate = (create_task)dlsym(_task_handle, "create");
                if(!pfcreate){
                    console::error("{} instance cannot be created", taskname);
                    dlclose(_task_handle);
                    _task_handle = nullptr;
                    return false;
                }
                _taskImpl = pfcreate();
                return true;
            }
            else {
                console::error("{} load error occured : {}", taskname, dlerror());
                dlclose(_task_handle);
                _task_handle = nullptr;
            }
        }
        else {
            console::error("Cannot find PATH in config file. You should define BIN_DIR.");
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
            console::error("timer create error");
    
        const unsigned long long nano = (1000000000L);
        _time_spec.it_value.tv_sec = nsec / nano;
        _time_spec.it_value.tv_nsec = nsec % nano;
        _time_spec.it_interval.tv_sec = nsec / nano;
        _time_spec.it_interval.tv_nsec = nsec % nano;

        if(timer_settime(_timer_id, 0, &_time_spec, nullptr)==-1)
            console::error("timer setting error");
    }

    //concreate process impl.
    void driver::do_process() {

        //signal set for threading
        sigset_t thread_sigmask;
        sigemptyset(&thread_sigmask);
        sigaddset(&thread_sigmask, SIG_RUNTIME_TRIGGER);
        sigaddset(&thread_sigmask, SIG_PAUSE_TRIGGER);
        sigaddset(&thread_sigmask, SIG_RESUME_TRIGGER);
        sigaddset(&thread_sigmask, SIG_STOP_TRIGGER);
        int _sig_no;

        while(1){
            sigwait(&thread_sigmask, &_sig_no);
            if(_sig_no==SIG_RUNTIME_TRIGGER){
                _taskImpl->set_status(task::status_d::WORKING);
                //auto t_now = std::chrono::high_resolution_clock::now();
                if(_taskImpl){
                    _taskImpl->execute();
                }
                //kauto t_elapsed = std::chrono::high_resolution_clock::now();
                //spdlog::info("Processing Time : {} sec", std::chrono::duration<double, std::chrono::seconds::period>(t_elapsed - t_now).count());
            }
            else if(_sig_no==SIG_PAUSE_TRIGGER) {
                sigdelset(&thread_sigmask, SIG_RUNTIME_TRIGGER);
                _taskImpl->set_status(task::status_d::PAUSED);
                if(_taskImpl){
                    _taskImpl->pause();
                }

            }
            else if(_sig_no==SIG_RESUME_TRIGGER) {
                _taskImpl->set_status(task::status_d::WORKING);
                if(_taskImpl){
                    _taskImpl->resume();
                }
                sigaddset(&thread_sigmask, SIG_RUNTIME_TRIGGER);
            }

            _taskImpl->set_status(task::status_d::IDLE);    
        }
    }
} /* namespace */