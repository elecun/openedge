
#include "general.hpp"

namespace oe {
    device::device(const char* devname){

    }

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
}