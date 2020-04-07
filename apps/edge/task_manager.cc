
#include "task_manager.hpp"
#include <external/spdlog/spdlog.h>

namespace oe {
    namespace edge {

        task_manager::task_manager(){

        }

        task_manager::~task_manager(){
            this->uninstall();
        }

        bool task_manager::install(const char* taskname){
            if(!taskname)
                return false;

            //1. insert task into container with uuid
            spdlog::info("Installing {}", taskname);
            _container_map.insert(container_t_map::value_type(taskname, util::generate_uuid()));
            _task_container.insert(container_t::value_type(_container_map[taskname], new core::task_driver(taskname)));
            spdlog::info("{} UUID : {}", taskname, _container_map[taskname].str());
            _task_container[_container_map[taskname]]->configure();

            return true;
        }

        void task_manager::uninstall(const char* taskname){
            spdlog::info("Uninstalling {}", taskname);
            if(!taskname){
                for(container_t::iterator itr = _task_container.begin(); itr!=_task_container.end();++itr){
                    delete itr->second;
                }
                _task_container.clear();
            }
            else {
                //delete _task_container[_container_map[taskname]];
                _task_container[_container_map[taskname]]->cleanup();
            }
        }

        void task_manager::run(const char* taskname){
            if(!taskname){
                for(container_t::iterator itr = _task_container.begin(); itr!=_task_container.end();++itr){
                    itr->second->execute();
                }
            }
            else {
                if(_task_container.find(_container_map[taskname])!=_task_container.end())
                    _task_container[_container_map[taskname]]->execute();
            }
            
        }

        void task_manager::cleanup(const char* taskname){
            _task_container[_container_map[taskname]]->cleanup();
        }

        oe::core::task_driver* task_manager::get_driver(const char* taskname){
            return nullptr;
        }



    } //namespace edge
} //namespace oe