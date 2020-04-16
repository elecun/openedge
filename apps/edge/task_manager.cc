
#include "task_manager.hpp"
#include <3rdparty/spdlog/spdlog.h>

namespace oe::edge {

    task_manager::task_manager(){

    }

    task_manager::~task_manager(){
        this->uninstall();
    }

    bool task_manager::install(const char* taskname){
        if(!taskname){
            spdlog::warn("Task was not specified to be installed");
            return false;
        }

        spdlog::info("Installing {}...", taskname);

        //insert task into container with uuid
        //(todo) requries file existance in task respository path
        _container_map.insert(container_t_map::value_type(taskname, _uuid_gen.generate()));
        _task_container.insert(container_t::value_type(_container_map[taskname], new core::task_driver(taskname)));
        spdlog::info("{} is assigned to {}", _container_map[taskname].str(), taskname);
        if(!_task_container[_container_map[taskname]]->configure()){
            uninstall(taskname);
        }
        spdlog::info("Totally installed : {}", _task_container.size());
        return true;
    }

    void task_manager::uninstall(const char* taskname){
        if(!taskname){
            spdlog::info("Uninstalling all tasks...");
            //all tasks will be uninstalled
            for(container_t::iterator itr = _task_container.begin(); itr!=_task_container.end();++itr){
                delete itr->second;
            }
            _task_container.clear();
        }
        else {
            spdlog::info("Uninstalling {}...", taskname);
            _task_container[_container_map[taskname]]->cleanup();
            delete _task_container[_container_map[taskname]];
            _task_container.erase(_container_map[taskname]);
            _container_map.erase(taskname);
        }
    }

    void task_manager::run(const char* taskname){
        if(!taskname){
            for(container_t::iterator itr = _task_container.begin(); itr!=_task_container.end();++itr){
                itr->second->execute();
            }
        }
        else {
            container_t::iterator itr = _task_container.find(_container_map[taskname]);
            if(itr!=_task_container.end())
                _task_container[_container_map[taskname]]->execute();
            else
                spdlog::warn("{} is already installed", taskname);
        }
        
    }

    void task_manager::cleanup(const char* taskname){
        if(_task_container[_container_map[taskname]])
            _task_container[_container_map[taskname]]->cleanup();
    }

    oe::core::task_driver* task_manager::get_driver(const char* taskname){
        return nullptr;
    }

} //namespace oe::edge