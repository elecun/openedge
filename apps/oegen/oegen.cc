/**
 * @file oegen.cc
 * @author Byunghun Hwang (bh.hwang@iae.re.kr)
 * @brief Auto file(component, profile, config) generator for OpenEdge
 * @version 0.1
 * @date 2022-06-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */

 #include <3rdparty/cxxopts.hpp>
#include <unistd.h>
#include <csignal>
#include <sys/mman.h>
#include "instance.hpp"
#include <iostream>
#include <openedge/log.hpp>
#include <openedge/core.hpp>
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace oe;

 int main(int argc, char* argv[]){

    console::stdout_color_st("console");

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
        console::error("Signal Handling Error");
        ::terminate(); //if failed, do termination
    }

    if(pthread_sigmask(SIG_SETMASK, &sigmask, nullptr)!=0){ // signal masking for this thread(main)
        console::error("Signal Masking Error");
        ::terminate();
    }

  int optc = 0;
  string _conf_filename;  //configuration file

  while((optc=getopt(argc, argv, "s:c:i:u:lvh"))!=-1){
    switch(optc){
      case 'i': { /* task installation dynamically */
        console::info("install {}", optarg);
      }
      break;

      case 'u': { /* task termination dynamically */
        console::info("uninstall {}", optarg);
      }
      break;

      case 'l': { /* show task list */
        console::info("list of tasks");
      }
      break;

      case 'v': /* show version */
      {
        cout << fmt::format("{} (built {}/{})", _OE_VER_, __DATE__, __TIME__) << endl;
        exit(EXIT_SUCCESS);
      }
      break;

      case 'c': { /* read configuration file */
        _conf_filename = optarg;
        console::info("Openedge Ver. {} (built {}/{})", _OE_VER_, __DATE__, __TIME__);
        console::info("Load configuration file(*.config) : {}", _conf_filename);
      }
      break;

      case 's':{ /* stop task */

      }

      case 'h':
      default:
        cout << fmt::format("Openedge Ver. {} (built {}/{})", _OE_VER_, __DATE__, __TIME__) << endl;
        cout << "Usage: edge [-c Config file] [-i Task name] [-u Task name] [-l Task list] [-v version]" << endl;
        cout << "-c\t Run edge by config file" << endl;
        cout << "-v\t Print Edge version" << endl;
        exit(EXIT_FAILURE);
      break;
    }
  }

  try{
    if(!_conf_filename.empty())
      if(oe::app::initialize(_conf_filename.c_str())){
        oe::app::run();
        pause(); //wait until getting SIGINT
      }
  }
  catch(const std::exception& e){
    console::error("Exception : {}", e.what());
  }

  ::terminate();
  return EXIT_SUCCESS;

 }