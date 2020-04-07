
#include "instance.hpp"
#include <fstream>
#include <openedge/ext/json.hpp>
#include <external/spdlog/spdlog.h>
#include "exception.hpp"
#include <vector>
#include <string>
#include "task_container.hpp"
#include <openedge/core/task.hpp>
#include "task_manager.hpp"

using namespace std;
using json = nlohmann::json;

namespace oe {
    namespace edge {

        //initialize
        bool init(const char* conf_file){

            //1, load configurationf file
            json config;
            try {
                std::ifstream file(conf_file);
                file >> config;
            }
            catch(const json::exception& e){
                spdlog::error("{}", e.what());
                return false;
            }

            //2. getting default tasks from configuration file
            vector<string> default_task = config["tasks"]["default"].get<std::vector<string>>();
            
            //3. install task into task manager
            for(string& task:default_task){
                spdlog::info("installing {}", task);
                edge_task_manager->install(task.c_str());
            }
        
            return true;
        }

        //start edge
        void run(){
            spdlog::info("Run All tasks in Realtime");
            edge_task_manager->run();
        }

    } //namespace edge
} //namespace oe