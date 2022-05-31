

/**
 * @brief Openedge Task Manager
 * 
 */

#ifndef _OPENEDGE_CORE_TASKMANAGER_HPP_
#define _OPENEDGE_CORE_TASKMANAGER_HPP_

#include <openedge/arch/singleton.hpp>
#include <openedge/util/uuid.hpp>
#include <openedge/core/driver.hpp>
#include <openedge/core/task.hpp>
#include <string>
#include <unordered_map>

using namespace std;

namespace oe::core {

    class task_manager : public oe::arch::singleton<task_manager> {
        public:
            typedef unordered_map<util::uuid_t, task::driver*> task_container_t;

            task_manager();
            virtual ~task_manager();

            /**
             * @brief install task
             * 
             * @param taskname 
             * @return true 
             * @return false 
             */
            bool install(const char* taskname = nullptr);
            bool install(const char* taskname, oe::core::task::runnable* instannce = nullptr);
            bool install(const char* taskname, oe::core::task::runnable_nt* instance = nullptr);
            bool install(const char* taskname, oe::core::task::runnable_rt* instance = nullptr);
            void uninstall(const char* taskname = nullptr); //install without system configuration

            void run(const char* taskname = nullptr);
            void cleanup(const char* taskname = nullptr);

            int size() const { return _task_container.size(); }

            oe::core::task::driver* get_driver(const char* taskname = nullptr);
            oe::core::task::runnable* get_task(const char* taskname = nullptr);

        protected:
            task_manager(const task_manager&) = delete;
            task_manager& operator = (const task_manager&) = delete;

            private:
                task_container_t _task_container;
                unordered_map<string, util::uuid_t> _task_uid_map;
                util::uuid_generator _uuid_gen;
    }; //end class

} /* end oe::core namespace */

#endif