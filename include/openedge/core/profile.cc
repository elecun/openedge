
#include "profile.hpp"
#include <3rdparty/spdlog/spdlog.h>

using namespace std;



namespace oe::core {
    profile::profile(const char* path){
        try{
            std::ifstream file(path);
            file >> data;
            valid = true;
            
            if(data.find("custom")!=data.end())
                custom = this->data["custom"];
        }
        catch(const json::exception& e){
            data.clear();
            custom.clear();
            valid = false;
        }
    }
}   //namespace oe

