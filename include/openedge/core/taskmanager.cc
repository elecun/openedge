
#include "manager.hpp"
#include <openedge/log.hpp>

namespace oe::core {

    task_manager::task_manager() {

    }

    task_manager::~task_manager(){

    }

    bool task_manager::install(const char* taskname){
        static int ntasks = 0;
        if(ntasks>__TASKS_LIMITS__){
            console::error("Task Container is Full!! (LIMIT={})", __TASKS_LIMITS__);
            return false;
        }

        if(!taskname){
            console::warn("Task was not specified to be installed");
            return false;
        }

        string config_fullpath = registry->get<std::string>("CONFIG_DIR")+string(taskname);
        if(!util::exist(config_fullpath.c_str())){
            console::error("{} file does not exist", taskname);
            return false;
        }
            
        //insert task into container with uuid
        //(todo) requries file existance in task respository path
        _container_map.insert(taskContainer_map::value_type(taskname, _uuid_gen.generate()));
        _task_container.insert(taskContainer_t::value_type(_container_map[taskname], new core::task::driver(taskname)));

        console::info("Installed {} Component(UUID:{})", taskname, _container_map[taskname].str());

        if(!_task_container[_container_map[taskname]]->configure()){
            uninstall(taskname);
        }
        return true;
    }

    bool task_manager::install(const char* taskname, oe::core::task::runnable* instannce){
        
        //check task name existance
        if(!taskname){
            console::warn("Task name was not specified. It must be required.");
            return false;
        }

        //check instance
        _task_uid_map.insert(unordered_map<string, util::uuid_t>::value_type(taskname, _uuid_gen.generate()));
        if(instance){ //static install
            _task_container.insert(unordered_map<util::uuid_t, task::driver*>::value_type(_task_uid_map[taskname], new task::driver(instance)));
        }
        else { //dynamic install
            _task_uid_map.insert(unordered_map<util::uuid_t, task::driver*>::value_type(_task_uid_map[taskname], new core::task::driver(taskname)));
        }
        console::info("Installed {} Component", taskname);
        if(!_task_container[_task_uid_map[taskname]]->configure()){
            uninstall(taskname);
        }

        return true;
    }

    bool task_manager::install(const char* taskname, oe::core::task::runnable_nt* instance){

    }

    bool task_manager::install(const char* taskname, oe::core::task::runnable_rt* instance){

    }

    void task_manager::uninstall(const char* taskname){
        if(!taskname){
            if(_task_container.size()>0){
                console::info("Uninstalling all tasks...");
                //all tasks will be uninstalled
                for(taskContainer_t::iterator itr = _task_container.begin(); itr!=_task_container.end();++itr){
                    itr->second->cleanup();
                    delete itr->second;
                }
                _task_container.clear();
                _container_map.clear();
            }
        }
        else {
            console::info("Uninstalling {}...", taskname);
            _task_container[_container_map[taskname]]->cleanup();
            delete _task_container[_container_map[taskname]];
            _task_container.erase(_container_map[taskname]);
            _container_map.erase(taskname);
        }
    }

    void task_manager::run(const char* taskname){
        if(!taskname){
            for(taskContainer_t::iterator itr = _task_container.begin(); itr!=_task_container.end();++itr){
                itr->second->execute();
            }
        }
        else {
            taskContainer_t::iterator itr = _task_container.find(_container_map[taskname]);
            if(itr!=_task_container.end())
                _task_container[_container_map[taskname]]->execute();
            else
                console::warn("{} is already installed", taskname);
        }
        
    }

    void task_manager::cleanup(const char* taskname){
        if(_task_container[_container_map[taskname]])
            _task_container[_container_map[taskname]]->cleanup();
    }

    oe::core::task::driver* task_manager::get_driver(const char* taskname){
        return nullptr;
    }

} //namespace