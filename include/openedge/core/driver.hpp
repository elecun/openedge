
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
            namespace task {

                typedef enum timer_type_t : unsigned int {
                    PERIODIC = 0,
                } timer_type;

                typedef struct task_stat_t {
                    unsigned long jitter;   //max jitter
                    unsigned long overrun;  //overrun counter
                } task_stat;

                class driver {
                    public:
                        driver(const char* taskname);
                        virtual ~driver();

                        //same interface of task
                        bool configure();
                        void execute();
                        void cleanup();

                        const char* getTaskname() const { return _taskImpl->_taskname.c_str(); }
                        //task::runnable* getTask() const { return _taskImpl; }

                    private:
                        bool load(const char* taskname);
                        void unload();
                        void do_process();

                        void set_rt_timer(unsigned long long nsec);

                    private:
                        task::runnable* _taskImpl = nullptr;    //concrete implementation
                        void* _task_handle = nullptr;   //for dl
                        std::thread* _ptrThread = nullptr;
                        std::mutex _mutex;

                        timer_t _timer_id = 0;
                        struct sigevent _sig_evt;
                        struct itimerspec _time_spec;
                        task_stat_t _stat;

                };
            } //namespace task
    } //namespace core
} //namespace oe

#endif