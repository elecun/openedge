
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

            spdlog::info("Process ID = {}", getpid());

            json config;
            try {
                std::ifstream file(conf_file);
                file >> config;
            }
            catch(const json::exception& e){
                spdlog::error("{}", e.what());
                return false;
            }

            vector<string> default_tasks = config["tasks"]["default"].get<std::vector<string>>();
            for(string& task:default_tasks){
                edge_task_manager->install(task.c_str());
            }
        
            return true;
        }

        //start edge
        void run(){
            edge_task_manager->run();
        }

        void cleanup(){
            edge_task_manager->uninstall();
        }

    } //namespace edge
} //namespace oe