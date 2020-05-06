
/**
 * @file    global.hpp
 * @brief   
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */


#ifndef _OPENEDGE_CORE_GLOBAL_HPP_
#define _OPENEDGE_CORE_GLOBAL_HPP_

#include <map>
#include <string>
#include <openedge/arch/singleton.hpp>
#include <3rdparty/spdlog/spdlog.h>

using namespace std;

namespace oe::edge {
    namespace global {

        class path : public oe::arch::singleton<path> {
            public:
                path() = default;
                virtual ~path() = default;

                string& operator[](const string& key) { return _path[key]; }

                void add(const char* key, const char* value){
                    if(_path.find(key)==_path.end()){
                        _path.insert(map<string, string>::value_type(key, value));
                    }
                }

                void add(const string& key, const string& value){
                    if(_path.find(key)==_path.end())
                        _path.insert(map<string, string>::value_type(key, value));
                }

                const char* get(const char* key) {
                    if(_path.find(key)!=_path.end())
                        return _path[key].c_str();
                    return "./";
                }

                const string& get(const string& key) {
                    if(_path.find(key)!=_path.end())
                        return _path[key].c_str();
                    return "./";
                }
            private:
                std::map<std::string, std::string> _path;
        }; //class path
        
    } //namespace global
} //namespace global

#define edge_path oe::edge::global::path::instance()

#endif