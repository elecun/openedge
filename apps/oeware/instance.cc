
#include "instance.hpp"
#include <openedge/core/task.hpp>
#include <external/json.hpp>
#include <fstream>
#include <vector>
#include <external/spdlog/spdlog.h>
#include "manager.hpp"

using json = nlohmann::json;

namespace oe {
    namespace ware {

        //oeware initialization
        bool init(const char* config_file) {

            //read configurations
            vector<string> default_tasks;
            
            //1. load configuration file
            try {
                std::ifstream file(config_file);
                json config;
                file >> config;
                
                for (json::iterator it = config["tasks"]["default"].begin(); it != config["tasks"]["default"].end(); ++it) {
                    if(it.value().is_string())
                        default_tasks.push_back(it.value());
                }
            }
            catch(nlohmann::json::exception& e){
                spdlog::error("Exception Message : {}({})", e.what(), e.id);
                return false;
            }

            //2. load default system tasks
            for(const string& task:default_tasks){
                spdlog::info("Load tasks : {}", task);
            }

            //2. find task in local & remote repository
            //oe::repo::container("local")

            //3. if found, install
            oe::task::container

            return true;
        }

        void start(void){

            
        }

    } //namespace ware
} //namespace oe
