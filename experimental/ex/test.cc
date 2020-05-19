

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
   
typedef struct fastEthernetHeader {
                uint8_t     companyID[10] = {0, };
                uint16_t    plcInfo;
                uint8_t     cpuInfo;
                uint8_t     sof;    //source of frame(->plc:0x33, <-plc:0x11)
                uint16_t    invokeId;
                uint16_t    length;
                uint8_t     position;
                uint8_t     reserved;
                bool test(){
                     const int a[] = {1,2,3};
                     if(companyID[0]==0){
                         int j = a[0];
                         printf("%d",j);
                        return true;
                     }
                        
                    return false;
                }
            };

    fastEthernetHeader test;

            printf("%d", sizeof(test));
    

    return 0;
}