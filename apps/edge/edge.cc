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

#include <3rdparty/cxxopts.hpp>

#include <csignal>
#include <sys/mman.h>
#include "instance.hpp"
#include <iostream>
#include <openedge/log.hpp>
#include <openedge/core.hpp>

using namespace std;

void terminate() {
  oe::app::cleanup();
  console::info("Successfully terminated");
  exit(EXIT_SUCCESS);
}

void cleanup(int sig) {
  switch(sig){
    case SIGSEGV: { console::info("Invalid access to storage"); } break;
    case SIGABRT: { console::info("Abnormal termination"); } break;
    case SIGKILL: { console::info("Process killed"); } break;
    case SIGBUS: { console::info("Bus Error"); } break;
    case SIGTERM: { console::info("Termination requested"); } break;
    case SIGINT: { console::info("interrupted"); } break;
    default:
      console::info("Cleaning up the program");
  }
  ::terminate(); 
}


int main(int argc, char* argv[])
{
  console::stdout_color_st("console");

  const int signals[] = { SIGINT, SIGTERM, SIGBUS, SIGKILL, SIGABRT, SIGSEGV };

  signal(SIGINT, cleanup);
	signal(SIGTERM, cleanup);
  signal(SIGBUS, cleanup);
  signal(SIGKILL, cleanup);
  signal(SIGABRT, cleanup);
  signal(SIGSEGV, cleanup);

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

  mlockall(MCL_CURRENT|MCL_FUTURE); //avoid swaping

  cxxopts::Options options(argv[0], "-  Options");
	options.add_options()
        ("c,config", "Load Configuration File(*.config)", cxxopts::value<std::string>(), "File Path") //require rerun avoiding
        ("i,install", "Install RT Task", cxxopts::value<std::string>(), "RT Task Component")
        ("u,unintall", "Uninstall RT Task", cxxopts::value<std::string>(), "RT Task Component")
        ("v,version", "Openedge Service Engine Version")
        ("h,help", "Print Usage");
       
  try
  {
    auto args = options.parse(argc, argv);
    
    if(args.count("version")) { cout << _OE_VER_ << endl; ::terminate(); }
    else if(args.count("install")) { cout << "Not Support yet" << endl; ::terminate(); }
    else if(args.count("uninstall")) { cout << "Not Support yet" << endl; ::terminate(); }
    else if(args.count("help")) { cout << options.help() << endl; ::terminate(); }
    else if(args.count("config")){
      string _conf = args["config"].as<std::string>();

      console::info("Starting Openedge Service Engine {} (built {}/{})", _OE_VER_, __DATE__, __TIME__);
      console::info("Load Configuration File : {}", _conf);

      //run task engine
      if(oe::app::initialize(_conf.c_str()))
          oe::app::run();
      
      pause(); //wait until getting SIGINT
    }
  }
  catch(const cxxopts::OptionException& e){
    console::error("Argument parse exception : {}", e.what());
  }

  ::terminate();
  return EXIT_SUCCESS;
}