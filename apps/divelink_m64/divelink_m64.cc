/**
 * @file divelink_m64.cpp
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 * @brief M64 Acoustic Model Control Middleware for Divelink
 * @version 0.1
 * @date 2022-05-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include <3rdparty/cxxopts.hpp>
#include <3rdparty/json.hpp>
#include <3rdparty/spdlog/spdlog.h>
#include <3rdparty/spdlog/sinks/stdout_color_sinks.h>
#include <openedge/common/bus.hpp>
#include <csignal>
#include <string>
#include <openedge/core/task.hpp>
#include <include/core/taskmanager.hpp>
#include "./task/tasks.hpp"

using namespace std;
using namespace nlohmann;
using namespace meshlink;

namespace divelink {

    /* before destroy */
    void destroy(){

        spdlog::info("Successfully destroyed");
        exit(EXIT_SUCCESS);
    }

    /* cleanup */
    void cleanup(int sig){
        switch(sig){
            case SIGSEGV: { spdlog::warn("Segmentation violation"); } break;
            case SIGABRT: { spdlog::warn("Abnormal termination"); } break;
            case SIGKILL: { spdlog::warn("Process killed"); } break;
            case SIGBUS: { spdlog::warn("Bus Error"); } break;
            case SIGTERM: { spdlog::warn("Termination requested"); } break;
            case SIGINT: { spdlog::warn("interrupted"); } break;
            default:
            spdlog::info("Cleaning up the program");
        }
    }

    /* catch signal */
    void set_signal(){
        const int signals[] = { SIGINT, SIGTERM, SIGBUS, SIGKILL, SIGABRT, SIGSEGV };
        for(const int& s:signals)
            signal(s, cleanup);

        //signal masking
        sigset_t sigmask;
        if(!sigfillset(&sigmask)){
            for(int signal:signals)
                sigdelset(&sigmask, signal); //delete signal from mask
        }
        else {
            spdlog::error("Signal Handling Error");
            divelink::destroy(); //if failed, do termination
        }

        if(pthread_sigmask(SIG_SETMASK, &sigmask, nullptr)!=0){ // signal masking for this thread(main)
            spdlog::error("Signal Masking Error");
            divelink::destroy();
        }
    } /* set_signal */
} /* namespace */

int main(int argc, char* argv[])
{
    spdlog::stdout_color_st("console");

    /* variables (with default) */
    int optc = 0;
    string _device_port = "/dev/ttyS0";
    string _mqtt_broker = "127.0.0.0";
    int _baudrate = 9600;

    /* command options */
    while((optc=getopt(argc, argv, "p:b:s:h"))!=-1)
    {
        switch(optc){
            case 'p': { /* Device Port */
                _device_port = optarg;
            } break;
            case 'b': { /* Baudrate */
                _baudrate = atoi(optarg);
            } break;
            case 's': { /* Share Data */
                _mqtt_broker = optarg;
            } break;
            case 'h':
            default:{
                cout << fmt::format("Divelink Middleware for M64 Acoustic Modem (built {}/{})", __DATE__, __TIME__) << endl;
                cout << "Usage: divelink_m64 [-p port] [-b baudrate] [-s Broker Address to share data] [-h help]" << endl;
                exit(EXIT_FAILURE);
            }
            break;
        }
    }

    /* show arguments by user */
    spdlog::info("> set device port : {}", _device_port);
    spdlog::info("> set port baudrate : {}", _baudrate);

    try {
        unique_ptr<meshlink::core::taskmanager> _manager(new meshlink::core::taskmanager());
        _manager->install("waterlink_m64", new app::task_m64_control(_device_port.c_str(), _baudrate));

        ::pause();

    }
    catch(const std::exception& e){
        spdlog::error("Exception occurred : {}", e.what());
    }

    divelink::destroy();

    return 0;
}