
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


#include <cxxabi.h>
inline const char* __demangle__(const char* name){
    int status;
    char* demangled = abi::__cxa_demangle(name, 0, 0, &status);
    std::string str(demangled);
    return str.c_str();
}
#define _THIS_COMPONENT_ __demangle__((const char*)typeid(*this).name()).get()

using namespace std;

namespace oe {
    namespace core { class iService; }
    namespace core::task {
            
            //realtime task interface
            class runnable {

                friend class oe::core::task::driver;
                
                public:
                    enum class TASK_STATUS : int { STOPPED=0, RUNNING, PAUSED };

                    virtual TASK_STATUS getStatus() { return _status; }
                    void setStatus(TASK_STATUS s) { _status = s;  }
                    
                    //common interface
                    virtual void execute() = 0;
                    virtual bool configure() = 0;
                    virtual void cleanup() = 0;

                // protected:
                //     explicit runnable(const char* taskname):_taskname(taskname), _status(TASK_STATUS::STOPPED){
                //     }

                protected:
                    typedef struct service_t {
                        void* handle = nullptr;
                        core::iService* ptrService = nullptr;
                    } serviceAccess;
                    map<string, serviceAccess> serviceContainer;

                    core::profile* _profile = nullptr;   //task profile (raw)
                    std::string _taskname;
                    TASK_STATUS _status;
            }; //class runnable

    } //namespace core

    typedef oe::core::task::runnable*(*create_task)(void);
    typedef void(*release_task)(void);

    #define EXPORT_RT_TASK_API extern "C" { oe::core::task::runnable* create(void); void release(void); }

} //namespace oe

#endif