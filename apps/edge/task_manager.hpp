

/**
 * @file    task_manager.hpp
 * @brief   task manager class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */


#ifndef _OPENEDGE_CORE_MANAGER_HPP_
#define _OPENEDGE_CORE_MANAGER_HPP_

#include <openedge/arch/singleton.hpp>
#include "task_manager.hpp"
#include <map>
#include <string>
#include <openedge/core/task.hpp>
#include <openedge/util/uuid.hpp>
#include <openedge/core/driver.hpp>

using namespace std;

namespace oe {
    namespace edge {

        class task_manager : public oe::arch::singleton<task_manager> {
            public:

            typedef std::map<util::uuid_t, oe::core::task_driver*> container_t;
            typedef std::map<string, util::uuid_t> container_t_map;

            task_manager();
            ~task_manager();
            class task_container;

            bool install(const char* taskname = nullptr);
            void uninstall(const char* taskname = nullptr);
    
            void run(const char* taskname = nullptr);
            void cleanup(const char* taskname = nullptr);

            oe::core::task_driver* get_driver(const char* taskname = nullptr);
            oe::core::rt_task::runnable* get_task(const char* taskname = nullptr);

            protected:
            task_manager(const task_manager&) = delete;
	        task_manager& operator = (const task_manager&) = delete;

            private:
                container_t _task_container;
                container_t_map _container_map;
                util::uuid_generator _uuid_gen;

        };

    } //namespace edge
} //namespace oe

#define edge_task_manager oe::edge::task_manager::instance()

#endif