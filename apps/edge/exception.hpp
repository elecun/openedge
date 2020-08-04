
/**
 * @file    exception.hpp
 * @brief   exception code for edge application
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _EDGE_EXCEPTION_HPP_
#define _EDGE_EXCEPTION_HPP_

#include <exception>
#include <string>
#include <openedge/core/exception.hpp>
#include "excode.hpp"


using namespace std;

namespace oe::edge {

    //exception class
    class exception : public oe::exception {
        exception(const edge::excode code, const char* err_msg){
            switch(code){
                case excode::EDGE_LOAD_CONFIG_FAIL: set(err_msg); break;
                default: set("Unexpected Exception");
            }
        }
        
    };//class core::exception

} //namespace oe::edge

#endif