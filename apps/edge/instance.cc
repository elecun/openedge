
#include "instance.hpp"
#include <fstream>
#include <openedge/ext/json.hpp>
#include <external/spdlog/spdlog.h>
#include "exception.hpp"

using namespace std;
using json = nlohmann::json;

namespace oe {
    namespace edge {

        //initialize
        bool init(const char* conf_file){
            json config;
            try {
                std::ifstream file(conf_file);
                file >> config;
            }
            catch(const json::exception& e){
                spdlog::error("{}",e.what());   //excode
                return false;
            }

            //load tasks
            json tasks = config["tasks"]["default"];
            for(json::iterator itr = tasks.begin(); itr!=tasks.end(); ++itr)
                spdlog::info("Default Task = {}", (*itr).get<std::string>());
            

            return true;
        }

        //start edge
        void run(){

        }

    } //namespace edge
} //namespace oe