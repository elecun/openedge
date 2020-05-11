
#include "profile.hpp"
#include <3rdparty/spdlog/spdlog.h>

using namespace std;



namespace oe::core {
    profile::profile(const char* path){
        try{
            std::ifstream file(path);
            file >> data;
            valid = true;
        }
        catch(const json::exception& e){
            data.clear();
            valid = false;
        }
    }
}   //namespace oe

