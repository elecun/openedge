

#include <iostream>
#include <ctime>
#include <string>
#include <cstring>

using namespace std;

const char uuid_key[] = {
            '0','1','2','3','4','5','6','7','8','9',
            'A','B','C','D','E','F','G','H','I','J',
            'K','L','M','N','O','P','Q','R','S','T',
            'U','V','W','X','Y','Z'
            };

typedef struct uuid_t {
            unsigned char uuid[16];
            uuid_t() {
                srand((unsigned int)time(nullptr));
            }
            uuid_t& operator=(const uuid_t& src){
                memcpy(this->uuid, src.uuid, sizeof(src.uuid));
                return *this;
            }

            void generate(){
                for(unsigned long n=0;n<sizeof(uuid);n++)               
                    uuid[n] = uuid_key[rand()%sizeof(uuid)];
            }
        } uuid;

        

        

int main(int argc, char* argv[])
{
    // uuid id = generate_uuid();
    // for(int i=0;i<16;i++)
    //     cout << id.uuid[i];
    // cout << endl;

    uuid_t id;
    for(int n=0;n<4;n++){
        id.generate();
        for(int i=0;i<16;i++)
            cout << id.uuid[i];
        cout << endl;
    }
    

    return 0;
}