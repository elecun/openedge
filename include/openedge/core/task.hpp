
/**
 * @file    task.hpp
 * @brief   Task abstraction class
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_TASK_HPP_
#define _OPENEDGE_CORE_TASK_HPP_

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <openedge/core/profile.hpp>
#include <any>
#include <openedge/core/dbus.hpp>

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
            
            // /**
            //  * @brief   runnable class interface for task
            //  */
            // class runnable {

            //     friend class oe::core::task::driver;
                
            //     public:
            //         enum class Status : int { STOPPED=0, WORKING, PAUSED, RESUMED, IDLE};

            //         virtual Status getStatus() { return status; }
            //         void setStatus(Status s) { status = s;  }
                    
            //         //task common interfaces (very important!!!!)
            //         virtual void execute() = 0;
            //         virtual bool configure() = 0;
            //         virtual void cleanup() = 0;
            //         virtual void pause() = 0;
            //         virtual void resume() = 0;

            //     protected:
            //         const core::profile* getProfile() { 
            //             assert(_profile!=nullptr);
            //             return _profile.get();
            //         }

            //     protected:
            //         typedef struct serviceHandle_t {
            //             public:
            //                 void* pfHandle { nullptr }; //component file handler
            //                 core::iService* pService { nullptr }; //service implementation ptr
            //                 bool isValid() { return (!pfHandle && !pService); }
            //                 const string getName() const { return name; }
            //                 serviceHandle_t(){}
            //                 serviceHandle_t(const char* svcname):name(svcname){}
            //             private:
            //                 string name {""};

            //         } serviceHandle;
            //         map<string /*service name*/, serviceHandle /*service handle*/> serviceContainer;

            //         // Fault type definition of Realtime System
            //         enum class fault_type_t : int {
            //             PERMANENT = 101,
            //             TRANSIENT,
            //             INTERMITTENT
            //         };

            //         // Fault level definition for openedge middleware
            //         enum class fault_level_t : int {
            //             NO_FAULT = 0,
            //             INFORM, // just notification only, no system reaction
            //             WARN,   // starts fault counting, log faults, no system reaction
            //             SERIOUS, // pause specific tasks
            //             CRITICAL = 0 // terminate all tasks (hard)
            //         };

            //         string taskname { "unknown" };
            //         Status status { Status::STOPPED };
            //         fault_level_t fault_level { fault_level_t::NO_FAULT };

            //     private:
            //         unique_ptr<core::profile> _profile;
            //         bool _check_jitter {false};
            //         bool _check_overrun {false};

            // }; //class runnable

            typedef struct _taskOptions {
                bool check_jitter = false;
                bool check_overrun = false;
            } taskOptions;

            enum class status_d : int { STOPPED=0, STOPPING, WORKING, PAUSED, IDLE };
            enum class rtype_d : int { NT = 0, RT, NP }; //runnable task type (NT=Non RT), NP(=Non Periodic)

            /**
             * @brief Runnable Class Base
             * 
             */
            class runnable {
                friend class oe::core::task::driver;

                public:
                    runnable() = default;
                    virtual ~runnable() = default;

                    /* interface functons */
                    virtual void execute() = 0;
                    virtual void stop() = 0;
                    virtual bool configure() = 0;
                    virtual void cleanup() = 0;
                    virtual void pause() = 0;
                    virtual void resume() = 0;
                    virtual const char* get_name() { return _taskname.c_str(); }

                    const status_d get_status() { return status; }
                    void set_status(status_d s) { status = s; }

                protected:
                    const core::profile* get_profile() { 
                        assert(_profile!=nullptr);
                        return _profile.get();
                    }

                protected:
                    status_d status { status_d::STOPPED };
                    rtype_d rtype;
                    unique_ptr<core::dbus::connector> databus;


                private:
                    unique_ptr<core::profile> _profile;
                    taskOptions _option;
                    string _taskname = {"noname"};
            };

            class runnable_np : public runnable {
                public:
                    runnable_np(){
                        this->rtype = rtype_d::NP;
                    }
                    virtual ~runnable_np() = default;
            };

            /**
             * @brief Runnable Non RT Task Base Class
             * 
             */
            class runnable_nt : public runnable {
                public:
                    runnable_nt(){
                        this->rtype = rtype_d::NT;
                    }
                    virtual ~runnable_nt() = default;

                    /**
                     * @brief Request data to other Non Realtime task
                     * 
                     * @param target_dst other task to process
                     * @param data data to pass the other task
                     * @return true successfully passed
                     * @return false failed to pass
                     */
                    //virtual bool request(string target_dst, std::any data, unsigned int priority) = 0; //bigger is higher

                    /**
                     * @brief message receive callback function
                     * 
                     * @param data 
                     */
                    virtual void on_request(std::any data) = 0;

                private:
                    unsigned int _task_priority = 0;

            }; /* class runnable for Non RT */

            /**
             * @brief Runnable RT Task Base Class
             * 
             */

            class runnable_rt : public runnable {
                public:
                    runnable_rt(){
                        this->rtype = rtype_d::RT;
                        //this->databus = make_unique<dbus::connector_zmq>();
                    }
                    virtual ~runnable_rt() = default;

                protected:
                    /* Fault Type Definition of RT System */
                    /**
                     * @brief Fault type Definitions
                     * PERMANENT : They persist indefinitely(or at least until repair) after their occurrence
                     * INTERMITTENT : They occur sporadically. This is a temporary situation that occurs sporadically such as loss of a relay contact.
                     * TRANSIENT : They occur in isolation such as electromagnetic interference.
                     */
                    enum class fault_type_d : int {
                        PERMANENT = 101,
                        TRANSIENT,
                        INTERMITTENT
                    };

                    /* Fault Alarm Level Definition */
                    enum class fault_level_d : int {
                        NONE = 0,   // Without Fault Alarm
                        INFORM,     // Notification only and No system reaction responsibility
                        WARN,       // Start Fault Counting and Logging Faults, but No system reaction responsibility
                        SERIOUS,    // Pause the fault task, but influence the effect to other tasks (Soft RT)
                        CRITICAL    // Terminate all tasks (Hard RT)
                    };

                protected:
                    fault_type_d    fault_type = fault_type_d::PERMANENT;
                    fault_level_d   fault_level = fault_level_d::NONE;

            }; /* class runnable for RT */

    } //namespace core

    typedef oe::core::task::runnable*(*create_task)(void);
    typedef void(*release_task)(void);

    #define EXPORT_TASK_API extern "C" {oe::core::task::runnable* create(void); void release(void);}

} //namespace oe

#endif