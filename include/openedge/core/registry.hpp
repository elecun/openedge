
/**
 * @file    registry.hpp
 * @brief   system registry (key-value paired memory map)
 * @author  Byunghun hwang@iae.re.kr
 */


#ifndef _OPENEDGE_CORE_REGISTRY_HPP_
#define _OPENEDGE_CORE_REGISTRY_HPP_

#include <any>
#include <string>
#include <unordered_map>
#include <openedge/arch/singleton.hpp>
#include <openedge/log.hpp>

using namespace std;

namespace oe::global {
    
    class registry : public oe::arch::singleton<registry> {
        public:
            registry() = default;
            virtual ~registry() = default;

            virtual bool insert(const string key, const any value){
                if(registry_container.find(key)==registry_container.end()){
                    registry_container.insert(std::pair<string, any>(key, value));
                    return true;
                }
                else {
                    console::warn("{} has already existed in the system registry.", key);
                    return false;
                }
            }

            template<typename _Type>
            _Type get(const string key){
                if(registry_container.find(key)!=registry_container.end())
                    return std::any_cast<_Type>(registry_container[key]);
                else
                    return nullptr;
            }

            bool find(const char* key){
                if(registry_container.find(key)==registry_container.end())
                    return false;
                return true;
            }

        private:
            unordered_map<string, any> registry_container;
    };
} //namespace

#define registry oe::global::registry::instance()

#endif