
/**
 * @file    task.hpp
 * @brief   Task abstraction  class
 * @author  Byunghun Hwang
 */

#ifndef _OPENEDGE_CORE_TASK_HPP_
#define _OPENEDGE_CORE_TASK_HPP_

#include <string>
#include <thread>
#include <memory>
#include <openedge/core/rt_timer.hpp>

using namespace std;

namespace oe {
    namespace core {






        //old
        typedef unique_ptr<std::thread> task;

        class abstractTask {
            public:
                abstractTask(const char* taskname);
                virtual ~abstractTask();

                const char* getTaskName() { return _taskname.c_str(); }

                void start();   //start task
                void stop();    //stop task

                virtual bool configure() = 0;
	            virtual void execute() = 0;
	            virtual void cleanup() = 0;

            private:
                string _taskname;
                bool _working; 
        };
    } //namespace core
} //namespace oe

#endif