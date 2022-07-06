
#include "instance.hpp"
#include <fstream>
#include <3rdparty/json.hpp>
#include "exception.hpp"
#include <vector>
#include <string>
#include <map>
#include <openedge/core/manager.hpp>
#include <sys/sysinfo.h>
#include <openedge/core/task.hpp>
#include <openedge/core/registry.hpp>
#include <openedge/log.hpp>

using namespace std;
using json = nlohmann::json;

namespace oe::manager {

    bool initialize_proc(){

        /* target system summarized info. */
        console::info("> Process ID = {}", getpid());
        console::info("> System CPUs = {}", get_nprocs());
        console::info("> System Clock Ticks = {}", sysconf(_SC_CLK_TCK));

        /* getting hostname */
        char hostname[256] = {0,};
        if(!gethostname(hostname, sizeof(hostname))){
            registry->insert("HOSTNAME", std::make_any<std::string>(hostname));
            console::info("> Target system hostname = {}", hostname);
        }
        else {
            
        }
        switch(err){
            case 0: {
                registry->insert("HOSTNAME", std::make_any<std::string>(hostname));
                console::info("> Target system hostname = {}", hostname);
            } break;
            case EINVAL: {
                console::warn("> Invalid hostname length");
            } break;
            case 
        }

        if(!gethostname(hostname, sizeof(hostname))){
            registry->insert("HOSTNAME", std::make_any<std::string>(hostname));
            console::info("> Target system hostname = {}", hostname);
        }
        
        else {
            std::string host = CONFIG_HOSTNAME.get<std::string>();
            spdlog::warn("Cannot be recognized the hostname. Default hostname({}) will be set.", host);
        }
            
        //install default tasks
        vector<string> default_tasks = CONFIG_TASKS.get<std::vector<string>>();
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