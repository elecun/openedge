

#ifndef _OPENEDGE_INCLUDE_EXCEPTION_HPP_
#define _OPENEDGE_INCLUDE_EXCEPTION_HPP_

#include <exception>
#include <string>

#include "excode.hpp"

using namespace std;

namespace oe {

    //common exception class
    class oe_exception : public std::exception {
        public:
            oe_exception() {}
            ~oe_exception() throw() { }

            virtual const char* what() const throw() {
                return exception_str.c_str();
            }
        protected:
            void set(const char* msg) { exception_str = msg; }
        private:
            string exception_str;
    };

    //core exception
    namespace core {
        class exception : public oe::oe_exception {
            exception(core::excode code, const char* err_msg = nullptr){
                switch(code){
                    case excode::OE_SIGACTION_FAIL: set("RT Trigger Signal Action failed"); break;
                    default: set("Unexpected Exception");
                }
            }
            
        };//class core::exception
    }
} //namespace oe

#endif