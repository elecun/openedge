
#include "profile.hpp"
#include <openedge/ext/json.hpp>
#include <fstream>
#include <external/spdlog/spdlog.h>

using json = nlohmann::json;
using namespace std;

namespace oe {
    namespace core {

        profile::profile(const char* profile_path){
            this->load(profile_path);
        }

        bool profile::load(const char* profile_path){
            json profile;
            try {
                std::ifstream file(profile_path);
                file >> profile;
            }
            catch(const json::exception& e){
                spdlog::error("{}", e.what());
                return false;
            }

            return true;

        }
        

    } //namespace core
} //namespace oe