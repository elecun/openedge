
#include "instance.hpp"
#include <fstream>
#include <3rdparty/json.hpp>
#include <3rdparty/spdlog/spdlog.h>
#include "exception.hpp"
#include <vector>
#include <string>
#include <openedge/core/task.hpp>
#include "task_manager.hpp"
#include "global.hpp"
#include <sys/sysinfo.h>

using namespace std;
using json = nlohmann::json;

namespace oe::edge {

    //initialize
    bool init(const char* conf_file){

        //show process information
        spdlog::info("* Process ID = {}", getpid());
        spdlog::info("* System CPUs : {}", get_nprocs());
        spdlog::info("* System Clock Ticks : {}", sysconf(_SC_CLK_TCK));

        //read app. configuration file
        json config;
        try {
            std::ifstream file(conf_file);
            file >> config;
        }
        catch(const json::exception& e){
            spdlog::error("{}", e.what());
            return false;
        }

        //add global path
        edge_path->add("config", config["environments"]["path"]["config"].get<string>().c_str());
        edge_path->add("task", config["environments"]["path"]["task"].get<string>().c_str());
        edge_path->add("service", config["environments"]["path"]["service"].get<string>().c_str());

        vector<string> default_tasks = config["tasks"]["default"].get<std::vector<string>>();
        
        for(string& task:default_tasks){
            edge_task_manager->install(task.c_str());
        }
        spdlog::info("Totally installed : {}", edge_task_manager->size());
    
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