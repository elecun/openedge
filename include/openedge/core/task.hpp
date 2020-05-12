
/**
 * @file    task.hpp
 * @brief   Task abstraction  class
 * @author  Byunghun Hwang
 */

#ifndef _OPENEDGE_CORE_TASK_HPP_
#define _OPENEDGE_CORE_TASK_HPP_

#include <string>
#include <map>
#include <vector>
#include <memory>
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
                    enum class Status : int { STOPPED=0, RUNNING, PAUSED };

                    virtual Status getStatus() { return _status; }
                    void setStatus(Status s) { _status = s;  }
                    
                    //common interface
                    virtual void execute() = 0;
                    virtual bool configure() = 0;
                    virtual void cleanup() = 0;

                protected:
                    const core::profile* getProfile() { 
                        assert(_profile!=nullptr);
                        return _profile.get();
                    }

                protected:
                    typedef struct service_t {
                        void* handle { nullptr };
                        core::iService* ptrService { nullptr };
                        bool is_valid() { return (!handle && !ptrService); }
                    } serviceHandle;
                    map<string /*service name*/, serviceHandle /*service handle*/> serviceContainer;

                    string _taskname { "Unknown" };
                    Status _status { Status::STOPPED };

                private:
                    unique_ptr<core::profile> _profile;
            }; //class runnable

    } //namespace core

    typedef oe::core::task::runnable*(*create_task)(void);
    typedef void(*release_task)(void);

    #define EXPORT_TASK_API extern "C" {oe::core::task::runnable* create(void); void release(void);}

} //namespace oe

#endif