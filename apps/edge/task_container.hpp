
/**
 * @file    task_container.hpp
 * @brief   task container class
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */


#ifndef _EDGE_TASK_CONTAINER_HPP_
#define _EDGE_TASK_CONTAINER_HPP_

#include <openedge/core/task.hpp>
#include <map>
#include <string>
#include <openedge/arch/singleton.hpp>

using namespace std;

namespace oe {
    namespace edge {

        typedef std::map<unsigned int, oe::core::rt_task::runnable*> container_t;

        class task_container {
            public:
                task_container() { }
                ~task_container() { 
                    clear(); 
                }

                bool add(oe::core::rt_task::runnable* t){
                    // if(_container.find(t->get_tid())==_container.end()){
                    //     _container.insert(container_t::value_type(1, t));
                    //     return true;
                    // }
                    return false;
                }

            private:
                //clear container
                void clear() {
                    for(auto& itr:_container){
                        if(itr.second){
                            delete itr.second;
                            itr.second = nullptr;
                        }
                    }
                }
            private:
                container_t _container;
        };

        //task container
        // class task_container : private oe::arch::singleton<task_container> {
        //     public:
        //         task_container() = default;
        //         ~task_container() { clear(); }

        //         bool add(oe::core::itask* t){
        //             // if(_container.find(t->get_tid())==_container.end()){
        //             //     _container.insert(container_t::value_type(1, t));
        //             //     return true;
        //             // }
        //             return false;
        //         }

        //     private:
        //         //clear container
        //         void clear() {
        //             for(auto& itr:_container){
        //                 if(itr.second){
        //                     delete itr.second;
        //                     itr.second = nullptr;
        //                 }
        //             }
        //         }
        //     private:
        //         container_t _container;
        // };

    } //namespace edge
} //namespace oe

#endif