

/**
 * @file    validation.hpp
 * @brief   for any validation
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_UTIL_VALIDATION_HPP_
#define _OPENEDGE_UTIL_VALIDATION_HPP_

#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>

//check for file existance
inline bool exist (const char* filepath) {
  struct stat buffer;   
  return (stat(filepath, &buffer) == 0); 
}

#endif