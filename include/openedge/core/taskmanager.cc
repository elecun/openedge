
#include <openedge/core/manager.hpp>
#include <openedge/log.hpp>
#include <openedge/util/validation.hpp>
#include <openedge/core/registry.hpp>
#include <openedge/core/def.hpp>
#include <filesystem>

namespace fs = std::filesystem;

namespace oe::core {

    task_manager::task_manager() {
        
    }

    task_manager::~task_manager(){
        if(_task_container.size()>0){
            for(task_container_t::iterator itr = _task_container.begin(); itr!=_task_container.end();++itr){
                itr->second->cleanup();
            }
            _task_container.clear();
        }
    }

    // bool task_manager::install(const char* taskname){
    //     static int ntasks = 0;
    //     if(ntasks>__TASKS_LIMITS__){
    //         console::error("Task Container is Full!! (LIMIT={})", __TASKS_LIMITS__);
    //         return false;
    //     }

    //     if(!taskname){
    //         console::warn("Task was not specified to be installed");
    //         return false;
    //     }

    //     string config_fullpath = registry->get<std::string>("CONFIG_DIR")+string(taskname);
    //     if(!util::exist(config_fullpath.c_str())){
    //         console::error("{} file does not exist", taskname);
    //         return false;
    //     }
            
    //     //insert task into container with uuid
    //     //(todo) requries file existance in task respository path
    //     _container_map.insert(taskContainer_map::value_type(taskname, _uuid_gen.generate()));
    //     _task_container.insert(taskContainer_t::value_type(_container_map[taskname], new core::task::driver(taskname)));

    //     console::info("Installed {} Component(UUID:{})", taskname, _container_map[taskname].str());

    //     if(!_task_container[_container_map[taskname]]->configure()){
    //         uninstall(taskname);
    //     }
    //     return true;
    // }

    bool task_manager::install(const char* taskname, oe::core::task::runnable* instance){

        /* static installation */
        if(instance){
            console::warn("Static installation is not support yet");
            return false;
        }

        /* dynamic installation (~ install from component file) */
        else {
            if(!taskname){
                console::warn("Task was not specified. It must be required.", taskname);
                return false;
            }

            // 1. check file existance
            if(registry->find(PATH_BIN_DIR)){
                fs::path _bin = registry->get<string>(PATH_BIN_DIR);
                fs::path _task = _bin /= fs::path{fmt::format("{}{}",taskname, FILE_EXT_TASK)};
                fs::path _profile = _bin /= fs::path{fmt::format("{}{}",taskname, FILE_EXT_PROFILE)};

                if(!fs::exists(_task) || !fs::exists(_profile)){
                    console::error("{} component files(*{}, *{}) do not exist.", taskname, FILE_EXT_TASK, FILE_EXT_PROFILE);
                    return false;
                }
                    
                console::info("Component Install : {}", _task.c_str());
                console::info("Profile Load : {}", _profile.c_str());
            }

            // 2. check for already working
            if(_task_uid_map.find(taskname)!=_task_uid_map.end()){
                console::error("{} is already installed. So it cannot be installed on same process.");
                return false;
            }

            // 3. insert into the container
            _task_uid_map.insert(map<string, util::uuid_t>::value_type(taskname, _uuid_gen.generate()));
            _task_container.insert(map<util::uuid_t, task::driver*>::value_type(_task_uid_map[taskname], new task::driver(taskname)));

            // 4. check load fail or success
            if(_task_container[_task_uid_map[taskname]]->good()){
                console::info("Successfully installed <{}>", taskname);
                if(!_task_container[_task_uid_map[taskname]]->configure()){
                    console::error("<{}> component has a problem to configure.", taskname);
                    this->uninstall(taskname);
                }
            }
            else {
                console::error("<{}> component load failed", taskname);
                this->uninstall(taskname);
            }
            
        }

        return true;
    }

    void task_manager::uninstall(const char* taskname){
        /* for all tasks */
        if(!taskname){
            if(_task_container.size()>0){
                for(task_container_t::iterator itr = _task_container.begin(); itr!=_task_container.end(); ++itr){
                    itr->second->cleanup();
                    delete itr->second;
                }
                _task_container.clear();
                _task_uid_map.clear();
            }
        }
        /* for specified task */
        else {
            if(_task_uid_map.find(taskname)==_task_uid_map.end()){
                console::warn("{} cannot be found to uninstall", taskname);
                return;
            }

            _task_container[_task_uid_map[taskname]]->cleanup();
            delete _task_container[_task_uid_map[taskname]];

            _task_container.erase(_task_uid_map[taskname]);
            _task_uid_map.erase(taskname);
        }
    }

    void task_manager::run(const char* taskname){
        /* for all tasks */
        if(!taskname){
            for(task_container_t::iterator itr = _task_container.begin(); itr!=_task_container.end();++itr){
                itr->second->execute();
            }
        }
        /* for specified task */
        else {
            if(_task_uid_map.find(taskname)==_task_uid_map.end()){
                console::warn("{} cannot be found to run", taskname);
                return;
            }

            _task_container[_task_uid_map[taskname]]->execute();
        }
        
    }

    void task_manager::cleanup(const char* taskname){
        /* for all tasks */
        if(!taskname){
            for(task_container_t::iterator itr = _task_container.begin(); itr!=_task_container.end();++itr){
                itr->second->cleanup();
            }
        }
        /* for specified task */
        else {
            if(_task_uid_map.find(taskname)==_task_uid_map.end()){
                _task_container[_task_uid_map[taskname]]->cleanup();
            }
        }
    }

    oe::core::task::driver* task_manager::get_driver(const char* taskname){
        if(!taskname){
            console::error("It must be specified.");
        }
        else{
            if(_task_uid_map.find(taskname)==_task_uid_map.end()){
                return _task_container[_task_uid_map[taskname]];
            }
        }
        return nullptr;
    }

} //namespace