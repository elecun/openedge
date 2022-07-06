
#include "instance.hpp"
#include <fstream>
#include <3rdparty/json.hpp>
#include <3rdparty/spdlog/spdlog.h>
#include <vector>
#include <string>
#include <map>
#include <sys/sysinfo.h>
#include <openedge/core/task.hpp>       //task component
#include <openedge/core/registry.hpp>   //registry container
#include <openedge/util/validation.hpp> //file existance check
#include <openedge/core/manager.hpp>
#include <openedge/core/def.hpp>

using namespace std;
using json = nlohmann::json;

namespace oe::app {

    /**
     * @brief application  initialize
     * 
     * @param conf_file configuration file to load
     * @return true if successfully initialized
     * @return false if failed initialize
     */
    bool initialize(const char* conf_file){

        /* system information summary */
        spdlog::info("> Process ID = {}", getpid());
        spdlog::info("> System CPUs = {}", get_nprocs());
        spdlog::info("> System Clock Ticks = {}", sysconf(_SC_CLK_TCK));

        /* Configuration file existance and parsing error check */
        json config;
        try {
            if(!util::exist(conf_file)){
                spdlog::error("Configuration file does not exist");
                return false;
            }

            std::ifstream file(conf_file);
            file >> config;
        }
        catch(const json::exception& e){
            spdlog::error("Configuration file load failed : {}", e.what());
            return false;
        }
        catch(std::ifstream::failure& e){
            spdlog::error("Configuration file load failed : {}", e.what());
            return false;
        }

        /* Set Environments */
        if(config.find(CONFIG_ENV_KEY)!=config.end()){
            
            /* Set path (Absolute) */
            if(config[CONFIG_ENV_KEY].find(CONFIG_PATH_KEY)!=config[CONFIG_ENV_KEY].end()){
                for(json::iterator it = config[CONFIG_ENV_KEY][CONFIG_PATH_KEY].begin(); it != config[CONFIG_ENV_KEY][CONFIG_PATH_KEY].end(); ++it){
                    if(it.value().is_string()){
                        registry->insert(it.key(), std::make_any<std::string>(config[CONFIG_ENV_KEY][CONFIG_PATH_KEY][it.key()].get<std::string>()));
                        console::info("+ Register Path : {}={}", it.key(),(registry->get<std::string>(it.key())));
                    }
                }
            }   
        }
            
        /* required */
        if(config.find(CONFIG_REQ_KEY)!=config.end()){

            /* tasks */
            if(config[CONFIG_REQ_KEY].find(CONFIG_TASKS_KEY)!=config[CONFIG_REQ_KEY].end()){
                vector<string> required_tasks = config[CONFIG_REQ_KEY][CONFIG_TASKS_KEY].get<std::vector<string>>();
                for(string& task:required_tasks){
                    manager->install(task.c_str());
                }
                spdlog::info("Totally installed : {}", manager->size());
            }
        }
        return true;
    }

    /* Run all the installed task */
    void run(){
        manager->run();
    }

    /* stop & uninstall the tasks */
    void cleanup(){
        manager->uninstall();
    }

} //namespace edge