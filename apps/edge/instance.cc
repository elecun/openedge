
#include "instance.hpp"
#include <external/json.hpp>
#include <external/spdlog/spdlog.h>
#include <fstream>


namespace oe {
    
    /**
     * @brief   read JSON formatted configuration file
     * @param   configuration file
     */
    bool init(const char* configfile){

        using json = nlohmann::json;
        json config;
        try {
            std::ifstream file(configfile);
            file >> config;
        }
        catch(json::exception& e){
            spdlog::error("{}() throws exception ({}){}", __func__, e.id, e.what());
            return false;
        }

        return true;
    }


    void run(void){

    }

} //namespace oe