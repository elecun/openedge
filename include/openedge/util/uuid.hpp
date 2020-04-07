

/**
 * @file    uuid.hpp
 * @brief   generate 16 byte char array
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */



#include <string>
#include <cstring>

using namespace std;

namespace oe {
    namespace util {

        typedef struct uuid_t {
            int8_t uuid[16];
            uuid_t& operator=(const uuid_t& src){
                memcpy(this->uuid, src.uuid, sizeof(src.uuid));
                return *this;
            }

            string str(){
                char _tmp[17] = {0,};
                memcpy(_tmp, this->uuid, sizeof(this->uuid));
                _tmp[16] = '\0';
                return string(_tmp);
            }

            bool operator<(const uuid_t& rhs) const {
                char _tmp1[sizeof(uuid)+1] = {0,}, _tmp2[sizeof(uuid)+1] = {0,};
                memcpy(_tmp1, this->uuid, sizeof(this->uuid)); _tmp1[sizeof(uuid)] = '\0';
                memcpy(_tmp2, rhs.uuid, sizeof(rhs.uuid));  _tmp2[sizeof(uuid)] = '\0';
                return string(_tmp1)<string(_tmp2);
            }

        } uuid;

        const char uuid_key[] = {
            '0','1','2','3','4','5','6','7','8','9',
            'A','B','C','D','E','F','G','H','I','J',
            'K','L','M','N','O','P','Q','R','S','T',
            'U','V','W','X','Y','Z'
            };

        uuid_t generate_uuid();
        
    } //namespace util
} //namespace oe