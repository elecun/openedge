
#include "network.hpp"
#include <openedge/log.hpp>
#include <fstream>
#include <chrono>
#include <list>
#include <sstream>

namespace oe::sys::perf {

    network_perf::network_perf(const char* netif, const char* procfile)
    :_procfile(procfile),network(netif){
        measure();
    }

    json network_perf::measure(){

        json data;
        
        try {
            ifstream file(_procfile);
            if(file.is_open()){
                list<string> identifier {"RXbytes", "RXpackets", "RXerrs", "RXdrop", "RXfifo", "RXframe", "RXcompressed", "RXmulticast", "TXbytes", "TXpackets", "TXerrs", "TXdrop", "TXfifo", "TXcolls", "TXcarrier", "TXcompressed" };


                //skip 2 lines
                string line;
                getline(file, line);
                getline(file, line);
                
                const string delims { ":\t ,"};
                while(getline(file, line)){
                    size_t beg, pos = 0;
                    while((beg = line.find_first_not_of(delims, pos)) != std::string::npos)
                    {
                        pos = line.find_first_of(delims, beg + 1);
                        line.substr(beg, pos - beg);
                        console::info("{}", line.substr(beg, pos - beg));
                    }
                }

            }

        } catch(ifstream::failure& e){
            console::error("Exception({}):{}", getInterfaceName(), e.what());
        }

        return data;
        
    }


    unsigned long network_perf::getTXbps(){

    }

    unsigned long network_perf::getTXbps_boot(){

    }

    unsigned long network_perf::getRXbps(){
        
    }

    unsigned long network_perf::getRXbps_boot(){
        
    }

}