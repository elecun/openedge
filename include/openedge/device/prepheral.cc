
#include "prepheral.hpp"

namespace oe {

    prepheral::prepheral(const char* pname):_pname(pname){

    }

    prepheral::~prepheral() {

    }

    unsigned short prepheral::read(unsigned char address){
        
        return 0x0000;
    }

}