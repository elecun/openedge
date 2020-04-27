
#include "instance.hpp"
#include <fstream>
#include <3rdparty/json.hpp>
#include <3rdparty/spdlog/spdlog.h>
#include "exception.hpp"
#include <vector>
#include <string>
#include <openedge/core/task.hpp>
#include "task_manager.hpp"
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