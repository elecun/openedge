

#ifndef _OPENEDGE_INCLUDE_EXCEPTION_HPP_
#define _OPENEDGE_INCLUDE_EXCEPTION_HPP_

#include <exception>
#include <string>

#include "excode.hpp"

using namespace std;

namespace oe {

    class exception : public std::exception {
        public:
            exception() {}
            ~exception() throw() { }

            virtual const char* what() throw() {
                return _exception_str.c_str();
            }
        protected:
            void set(const char* msg) { _exception_str = msg; }
        private:
            std::string _exception_str;
    };

    //core exception
    namespace core {
        class exception : public oe::exception {
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