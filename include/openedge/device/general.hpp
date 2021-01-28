/**
 * @file    general.hpp
 * @brief   general device class
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.01.27
 **/

#ifndef _OPENEDGE_DEVICE_GENERAL_HPP_
#define _OPENEDGE_DEVICE_GENERAL_HPP_

#include <map>
#include <string>

using namespace std;
 
namespace oe {
    
    class prepheral;
    class device {
        public:
            device(const char* devname):_devicename(devname){
                _fd = ::open(devname, O_RDWR); //open device file desc. only
            }
            virtual ~device() {
                if(_fd)
                    ::close(_fd);
            }

            /**
             * @brief   check if device is opened
             **/
            bool isOpen(){
                if(_fd<0)
                    return false;
                return true;
            }

            void addPrepheral(oe::prepheral* p, const char* pname){
                _ppContainer.insert(std::pair<string, oe::prepheral*>(pname, p));
            }

            oe::prepheral* getPrepheral(const char* pname){
                if(_ppContainer.find(pname)!=_ppContainer.end()){
                    return _ppContainer[pname];
                }
                return nullptr;
            }

        private:
            map<string, oe::prepheral*> _ppContainer;
            int _fd = -1;
            string _devicename = "";

    };

} /* namespace oe */

#endif