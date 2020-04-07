
/**
 * @file    driver.hpp
 * @brief   task driver class
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_TASK_DRIVER_HPP_
#define _OPENEDGE_CORE_TASK_DRIVER_HPP_

#include <openedge/core/task.hpp>
#include <string>
#include <thread>
#include <signal.h>
#include <condition_variable>
#include <mutex>

using namespace std;

namespace oe {
    namespace core {

        #define SIG_RUNTIME_TRIGGER SIGRTMAX

        typedef enum timer_type_t : unsigned int {
            PERIODIC = 0,
        } timer_type;
        
        class task_driver : public oe::core::rt_task::runnable {
            public:
                task_driver(const char* taskname);

                virtual ~task_driver();

                bool configure() override;
                void execute() override;
                void cleanup() override;

            private:
                bool load(const char* taskname);
                void unload();
                void proc();
                void call();

                void init_timer();
                void stop_timer();

                static void timer_handler(int signo, siginfo_t* info, void* context) {
                (reinterpret_cast<task_driver*>(info->si_value.sival_ptr))->call();
            }

            private:
                string _taskname;
                rt_task::runnable* _task_impl = nullptr;    //concrete implementation
                void* _task_handle = nullptr;   //for dl
                std::thread* _ptr_thread;
                std::condition_variable _cv;
                std::mutex _mutex;
                bool done = false;
                bool notified = false;

                timer_t _timer_id = 0;
                struct sigevent _sig_evt;
                struct sigaction _sig_act;
                struct itimerspec _time_spec;


        };
    } //namespace core
} //namespace oe

#endif