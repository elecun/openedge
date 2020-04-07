
/**
 * @file    singleton.hpp
 * @brief   singleton template class
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */


#ifndef _OPENEDGE_ARCH_SINGLETON_HPP_
#define _OPENEDGE_ARCH_SINGLETON_HPP_

#include <utility>

namespace oe {
    namespace arch {

        template<class _t>
        class singleton {
            public:
            // template<typename... Args>
            // static _t* instance(Args... args){
            //     if(!_instance){
            //         _instance = new _t(std::forward<Args>(args)...);
            //     }
            //     return _instance;
            // }

            static _t* instance(){
                if(!_instance){
                    _instance = new _t();
                }
                return _instance;
            }

            static void terminate() {
                if(_instance){
                    delete _instance;
                    _instance = nullptr;
                }
            }

            protected:
            singleton() = default;
            ~singleton() = default;
            singleton(const singleton&) = delete;   //for noncopyable
            singleton& operator=(const singleton&) = delete; //for noncopyable

            private:
            static _t* _instance;
        };

    template <class _t> _t* singleton<_t>::_instance = nullptr; //initialize

    } //namespace arch
} //namespace oe

#endif