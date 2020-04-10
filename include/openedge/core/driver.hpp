
/**
 * @file    driver.hpp
 * @brief   task driver class
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_TASK_DRIVER_HPP_
#define _OPENEDGE_CORE_TASK_DRIVER_HPP_

#include <openedge/core/task.hpp>
#include <openedge/core/profile.hpp>
#include <string>
#include <thread>
#include <signal.h>
#include <mutex>

using namespace std;

namespace oe {
    namespace core {

        typedef enum timer_type_t : unsigned int {
            PERIODIC = 0,
        } timer_type;

        typedef struct task_stat_t {
            unsigned long jitter;   //max jitter
            unsigned long overrun;  //overrun counter
        };

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
                void do_process();

                void set_rt_timer(long nsec);

            private:
                string _taskname;
                rt_task::runnable* _task_impl = nullptr;    //concrete implementation
                void* _task_handle = nullptr;   //for dl
                std::thread* _ptr_thread;
                std::mutex _mutex;
                bool done = false;
                bool notified = false;

                timer_t _timer_id = 0;
                struct sigevent _sig_evt;
                struct itimerspec _time_spec;
                task_stat_t _stat;

                oe::core::profile_t _profile;   //profile

        };
    } //namespace core
} //namespace oe

#endif