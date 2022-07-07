
/**
 * @file    profile.hpp
 * @brief   task profile reader
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */


#ifndef _OPENEDGE_PROFILE_HPP_
#define _OPENEDGE_PROFILE_HPP_

#include <fstream>
#include <map>
#include <vector>
#include <cstring>
#include <string>
#include <3rdparty/json.hpp>
#include <openedge/log.hpp>
#include <openedge/core/def.hpp>

using namespace std;
using json = nlohmann::json;

namespace oe::core {

    namespace task { 
        class runnable; 
        class driver;
    }

    class profile {

        friend class oe::core::task::driver;

        public:
            profile(const char* path);
            virtual ~profile() {
                data.clear();
            }

            bool is_valid() const { return valid; }

            /**
             * @brief Get the dumped JSON profile
             * 
             * @param key find by key
             * @return string dumped string
             */
            string get_dumped(const char* key = nullptr) const {
                if(key){
                    if(data.find(key)!=data.end()){
                        return data[key].dump();
                    }
                    else {
                        console::warn("{} key cannot be found.", key);
                    }
                }
                else {
                    return data.dump();
                }
                return string("{}");
            }

            /**
             * @brief get access to JSON profile data
             * 
             * @return const json* 
             */
            const json& get() const { 
                return data;
            };
            

        protected:
            json data;

        private:
            bool valid = true;

    }; //class profile interface

    

    // struct profile_t {
    //     char md5[32] = {0,};
    //     int cpu_affinity;   //working cpu
    //     unsigned long long cycle_ns; //cycle time in nanoseconds
    //     string taskname;
    //     string version;

    //     profile_t& operator=(const profile_t& other){
    //         memcpy(this->md5, other.md5, sizeof(this->md5));
    //         this->cpu_affinity = other.cpu_affinity;
    //         this->cycle_ns = other.cycle_ns;
    //         this->taskname = other.taskname;
    //         this->version = other.version;
    //         return *this;
    //     }
    // };


}   //namespace oe


#endif