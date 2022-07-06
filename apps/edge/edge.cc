/**
 * @file    edge.cc
 * @brief   OpenEdge Task Runtime Engine on Preemptive Realtime OS
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

/*
Copyright (c) 2020 Byunghun,Hwang
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:
The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#include <unistd.h>
#include <csignal>
#include <sys/mman.h>
#include "instance.hpp"
#include <iostream>
#include <openedge/log.hpp>
#include <openedge/core.hpp>
#include <stdexcept>
#include <iostream>
#include <string>
#include <3rdparty/cxxopts.hpp>
#include <vector>

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