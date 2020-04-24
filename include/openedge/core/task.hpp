
/**
 * @file    task.hpp
 * @brief   Task abstraction  class
 * @author  Byunghun Hwang
 */

#ifndef _OPENEDGE_CORE_TASK_HPP_
#define _OPENEDGE_CORE_TASK_HPP_

#include <string>
#include <map>
#include <openedge/core/profile.hpp>

using namespace std;

namespace oe {
    namespace core {
            class iService;
            //realtime task interface
            class rt_task {
                public:
                    enum class TASK_STATUS : int { STOPPED = 0, RUNNING, PAUSED };

                    virtual TASK_STATUS getStatus() { return status;  }
                    void setStatus(TASK_STATUS s) { status = s;  }
                    
                    class runnable {
                        public:
                            virtual void execute() = 0;
                            virtual bool configure() = 0;
                            virtual void cleanup() = 0;

                        protected:
                            typedef struct service_t {
                                void* handle;
                                oe::core::iService* ptrService;
                            } serviceAccess;
                            map<string, serviceAccess> serviceContainer;
                    };

                protected:
                    profile_t profile;
                    TASK_STATUS status;
            };

    } //namespace core

    typedef oe::core::rt_task::runnable*(*create_rt_task)(void);
    typedef void(*release_rt_task)(void);

    #define EXPORT_RT_TASK_API extern "C" { oe::core::rt_task::runnable* create(void); void release(void); }

} //namespace oe

#endif