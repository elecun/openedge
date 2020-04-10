
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