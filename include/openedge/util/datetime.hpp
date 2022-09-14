/**
 * @file datetime.hpp
 * @author Byunghun Hwang (bh.hwang#iae.re.kr)
 * @brief Datetime management
 * @version 0.1
 * @date 2022-09-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _OPENEDGE_UTIL_DATETIME_HPP_
#define _OPENEDGE_UTIL_DATETIME_HPP_

#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>

using namespace std;

namespace oe::util {

    inline const char* get_current_datetime(){
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S");
        
        return oss.str().c_str();
    }

} /* namespace */


#endif