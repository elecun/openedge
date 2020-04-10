
/**
 * @file    profile.hpp
 * @brief   task profile reader
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */


#ifndef _OPENEDGE_PROFILE_HPP_
#define _OPENEDGE_PROFILE_HPP_

#include <fstream>
#include <map>

using namespace std;

namespace oe {
    namespace core {

        typedef struct profile_t {
            char md5[32] = {0,};
            int affinity;
            unsigned long cycle_ns; //cycle time in nanoseconds
        };

        class profile {
            public:
                profile(const char* profile_path);

            private:
                bool load(const char* profile_path);

            private:
                

        }; //class profile

    } //namepspace core
}   //namespace oe


#endif