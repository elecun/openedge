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
            device(const char* devname);
            virtual ~device();
            
            bool open();
            bool isOpen();

            void addPrepheral(oe::prepheral* p);
            oe::prepheral* getPrepheral(const char* pname);
            

        private:
            map<string, oe::prepheral*> _ppContainer;
            int _fd = -1;
            string _devicename = "";

    };

} /* namespace oe */

#endif