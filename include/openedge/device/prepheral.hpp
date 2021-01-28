/**
 * @file    prepheral.hpp
 * @brief   prepheral device class (general)
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 * @date    2021.01.27
 **/

#ifndef _OPENEDGE_DEVICE_PREPHERAL_HPP_
#define _OPENEDGE_DEVICE_PREPHERAL_HPP_

#include <string>

using namespace std;
 
namespace oe {

    enum class prepheral_class : int {
        I2C = 100,
    };
    
    class prepheral {
        public:
            prepheral(const char* pname):_pname(pname){

            }

            virtual ~prepheral() {}

        private:
            string _pname;
    };

} /* namespace oe */

#endif