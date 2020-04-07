
/**
 * @file    task.hpp
 * @brief   Task abstraction  class
 * @author  Byunghun Hwang
 */

#ifndef _OPENEDGE_CORE_TASK_HPP_
#define _OPENEDGE_CORE_TASK_HPP_

#include <string>

using namespace std;

namespace oe {
    namespace core {

            //realtime task interface
            class rt_task {
                public:
                    class runnable {
                        public:
                            virtual void execute() = 0;
                            virtual bool configure() = 0;
                            virtual void cleanup() = 0;
                        protected:
                            unsigned long _period_ns = 1*1000*1000UL;  // default period : 1ms
                    };

                    void regist(runnable& rtk){
                        if(!_runnable)
                            _runnable = &rtk;
                    }

                    void release() {
                        _runnable = nullptr;
                    }

                private:
                    runnable* _runnable = nullptr;
            };

    } //namespace core

    typedef oe::core::rt_task::runnable*(*create_rt_task)(void);
    typedef void(*release_rt_task)(void);

    #define EXPORT_RT_TASK_API extern "C" { oe::core::rt_task::runnable* create(void); void release(void); }

} //namespace oe

#endif