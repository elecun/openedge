
/**
 * @file    profile.hpp
 * @brief   task profile reader
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */


#ifndef _OPENEDGE_PROFILE_HPP_
#define _OPENEDGE_PROFILE_HPP_

#include <fstream>
#include <map>
#include <cstring>
#include <string>

using namespace std;

namespace oe::core {

    typedef std::string jsonDump;

        struct profile_t {
            char md5[32] = {0,};
            int cpu_affinity;   //working cpu
            unsigned long long cycle_ns; //cycle time in nanoseconds
            string taskname;
            string version;

            profile_t& operator=(const profile_t& other){
                memcpy(this->md5, other.md5, sizeof(this->md5));
                this->cpu_affinity = other.cpu_affinity;
                this->cycle_ns = other.cycle_ns;
                this->taskname = other.taskname;
                this->version = other.version;
                return *this;
            }
        };

        class profile {
            public:
                profile(){}
                ~profile(){}

                bool load(const char* profile_path, profile_t& dest);

            private:


        }; //class profile

}   //namespace oe


#endif