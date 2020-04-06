
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

        class itask {
            public:
                itask(const char* taskname):_taskname(taskname){}
                virtual ~itask() {}

                const char* get_taskname() { return _taskname.c_str(); }

                virtual bool configure() = 0;
                virtual void execute() = 0;
                virtual void cleanup() = 0;

            private:
            string _taskname;
        };

        class rt_task : public itask {
            public:
                class runnable{
                    public:
                        virtual void execute() = 0;
                };
 
                virtual ~rt_task(){
                    this->release();
                }

                void regist(runnable& r) { if(!_runnable) _runnable = &r; }

                virtual bool configure() = 0;
                virtual void execute(){
                    if(_runnable) _runnable->execute();
                }
                virtual void cleanup() = 0;

                protected:
                    void release() {
                        if(_runnable)
                            _runnable = nullptr;
                    }
            private:
                runnable* _runnable = nullptr;
        };
        
    } //namespace core
} //namespace oe

#endif