
#include "profile.hpp"
#include <openedge/ext/json.hpp>
#include <fstream>
#include <3rdparty/spdlog/spdlog.h>

using json = nlohmann::json;
using namespace std;

namespace oe {
    namespace core {

        bool profile::load(const char* profile_path, profile_t& dest){
            json _profile;
            try {
                std::ifstream file(profile_path);
                file >> _profile;

                strncpy(dest.md5, _profile["md5"].get<string>().c_str(), sizeof(dest.md5));
                dest.cpu_affinity = _profile["info"]["cpu_affinity"].get<int>();
                dest.cycle_ns = _profile["info"]["cycle_ns"].get<unsigned long long>();
                dest.taskname = _profile["info"]["taskname"].get<string>();
                dest.version = _profile["info"]["version"].get<string>();
            }
            catch(const json::exception& e){
                spdlog::error("{}", e.what());
                return false;
            }
            return true;

        }
        

    } //namespace core
} //namespace oe