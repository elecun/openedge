
#include "general.hpp"
#include <fcntl.h>
#include <unistd.h>

using namespace std;

namespace oe {

    device::device(const char* devname):_devicename(devname){
        _fd = ::open(devname, O_RDWR);
    }

    device::~device() {
        if(_fd)
            ::close(_fd);

        for(map<string, oe::prepheral*>::iterator itr = _ppContainer.begin(); itr!=_ppContainer.end(); itr++){
            delete itr->second;
            itr->second = nullptr;
        }
    }

    bool device::open(){
        if(_fd<0)
            _fd = ::open(_devicename.c_str(), O_RDWR);
        return true;
    }

    bool device::isOpen(){
        if(_fd<0)
            return false;
        return true;
    }

    void device::addPrepheral(oe::prepheral* p){
        _ppContainer.insert(std::pair<string, oe::prepheral*>(_devicename, p));
    }

    oe::prepheral* device::getPrepheral(const char* pname){
        if(_ppContainer.find(pname)!=_ppContainer.end()){
            return _ppContainer[pname];
        }
        return nullptr;
    }

} /* namespace */