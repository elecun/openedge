
#include <ctime>
#include <string>
#include <cstring>
#include "uuid.hpp"

using namespace std;

namespace oe {
    namespace util {

        uuid_t generate_uuid(){
            srand((unsigned int)time(nullptr));
            uuid _id;
            for(unsigned long n=0;n<sizeof(_id.uuid);n++)
                _id.uuid[n] = uuid_key[rand()%sizeof(uuid_key)];

            return _id;
        }
        
    } //namespace util
} //namespace oe