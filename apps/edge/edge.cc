/**
 * @file edge.cc
 * @author Byunghun Hwang(bh.hwang@iae.re.kr)
 * @brief Openedge Engine on Openedge Framework for Non-RT/RT OS platform
 * @version 0.1
 * @date 2022-07-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <unistd.h>
#include <csignal>
#include <sys/mman.h>
#include <iostream>
#include <openedge/log.hpp>
#include <openedge/core.hpp>
#include <stdexcept>
#include <iostream>
#include <string>
#include <3rdparty/cxxopts.hpp>
#include <vector>
#include "instance.hpp"

using namespace std;
using namespace oe;

void terminate(){
    app::cleanup();
    console::info("Cleaned up");
    exit(EXIT_SUCCESS);
}

void cleanup(int sig) {
    switch(sig){
        case SIGSEGV: { console::warn("Segmentation violation"); } break;
        case SIGABRT: { console::warn("Abnormal termination"); } break;
        case SIGKILL: { console::warn("Process killed"); } break;
        case SIGBUS: { console::warn("Bus Error"); } break;
        case SIGTERM: { console::warn("Termination requested"); } break;
        case SIGINT: { console::warn("interrupted"); } break;
        default:
        console::info("Cleaning up the program");
    }
    ::terminate(); 
}

int main(int argc, char* argv[])
{
    string desc = fmt::format("Ver. {} (built {}/{})", _OE_VER_, __DATE__, __TIME__);
    cxxopts::Options options("OpenEdge Framework Engine", desc.c_str());
    options.add_options()
        ("c,config", "Application start with configuration file(*.conf)", cxxopts::value<string>())
        ("i,install", "Install Component(Task)", cxxopts::value<vector<string>>())
        ("u,uninstall", "Uninstall Component(Task)", cxxopts::value<vector<string>>())
        ("l,list", "Show list of installed components(Task)")
        ("h,help", "Print usage");

    auto optval = options.parse(argc, argv);
    if(optval.count("help")){
        std::cout << options.help() << std::endl;
        exit(EXIT_SUCCESS);
    }

    console::stdout_color_st("console");

    const int signals[] = { SIGINT, SIGTERM, SIGBUS, SIGKILL, SIGABRT, SIGSEGV };
    for(const int& s:signals)
        signal(s, cleanup);

    sigset_t sigmask;
    if(!sigfillset(&sigmask)){
        for(int signal:signals)
        sigdelset(&sigmask, signal); //delete signal from mask
    }
    else {
        console::error("Signal Handling Error");
        ::terminate(); //if failed, do termination
    }

    if(pthread_sigmask(SIG_SETMASK, &sigmask, nullptr)!=0){ // signal masking for main thread
        console::error("Signal Masking Error");
        ::terminate();
    }

    mlockall(MCL_CURRENT|MCL_FUTURE); //avoid memory swaping

    
    /* option arguments */
    string _config {""};
    vector<string> _comps;
    if(optval.count("config")){
        _config = optval["config"].as<string>();
    }
    else if(optval.count("install")){
        _comps = optval["install"].as<vector<string>>();
    }
    else if(optval.count("uninstall")){
        _comps = optval["uninstall"].as<vector<string>>();
    }

    try{
        if(!_config.empty())
        if(app::initialize(_config.c_str())){
            app::run();
            pause(); //wait until getting SIGINT
        }
    }
    catch(const std::exception& e){
        console::error("Exception : {}", e.what());
    }

    ::terminate();
    return EXIT_SUCCESS;
}