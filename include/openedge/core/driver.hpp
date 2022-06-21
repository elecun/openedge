
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
#include <3rdparty/zmq/zmq.hpp>
#include <3rdparty/zmq/zmq_addon.hpp>

using namespace std;


namespace oe::core {
    namespace task {

        //RT timer jitter data
        typedef struct _time_jitter_t {
            unsigned long long max {0};
            unsigned long long min {1000000000};
            void set(unsigned long long val){
                if(val>max) max=val;
                if(val<min) min=val;
            }   
        } time_jitter;

        class driver {
            public:
                driver(const char* taskname);
                driver(task::runnable* instance);
                virtual ~driver();

                bool configure();   /* drive a task to configure before execution */
                void execute();     /* drive a task to run periodically */
                void cleanup();     /* drive a task to terminate */
                void pause();       /* drive a task to pause*/
                void resume();      /* drive a paused task to resume */

                /* get the name of task */
                const char* getTaskname() { 
                    return _taskImpl->taskname.c_str();
                }

            private:
                bool load(const char* taskname);    /* load task with by name, true is success  */
                void unload();  /* unload task */
                void do_process();  /* call the concrete task execution */

                //set task time spec. 
                void set_rt_timer(unsigned long long nsec);

            private:
                task::runnable* _taskImpl = nullptr;    //concrete implementation
                void* _task_handle = nullptr;   //for dl
                std::thread* _ptrThread = nullptr;
                std::mutex _mutex;
                int _signalIndex {0};
                timer_t _timer_id {0};
                struct sigevent _sig_evt;
                struct itimerspec _time_spec;
                _time_jitter_t _jitter;
                bool _overrun { false };
                    

        };
    } //namespace task
} //namespace oe

#endif