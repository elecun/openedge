/**
 * @file    edge.cc
 * @brief   OpenEdge Service Engine on Preemptive Realtime OS
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
#include <3rdparty/spdlog/spdlog.h>
#include <3rdparty/spdlog/sinks/stdout_color_sinks.h>
#include <csignal> //for signal handling
#include <sys/mman.h> //for mlock
#include "instance.hpp"
#include <iostream>
#include <semaphore.h>  //for process synchronization (avoiding re-run)

#include <openedge/core/version.hpp>
#include <openedge/core/exception.hpp>  //exception by openedge
#include <openedge/core/global.hpp>

using namespace std;

//global variables
static sem_t* _running = SEM_FAILED;
const char* semname = "openedge";

void terminate() {
  oe::edge::cleanup();
  spdlog::info("Successfully terminated");
  exit(EXIT_SUCCESS);
}

void cleanup(int sig) { 
  sem_close(_running);
  sem_unlink(semname);
  ::terminate(); 
}


int main(int argc, char* argv[])
{
  spdlog::stdout_color_st("console");

  //termination signal event redirection
  signal(SIGINT, cleanup);
	signal(SIGTERM, cleanup);
  signal(SIGKILL, cleanup);
  signal(SIGABRT, cleanup);

  //signal masking
  sigset_t sigmask;
  if(!sigfillset(&sigmask)){
    sigdelset(&sigmask, SIGINT);  //delete signal from signal mask
    sigdelset(&sigmask, SIGTERM);
    sigdelset(&sigmask, SIGKILL);
    sigdelset(&sigmask, SIGABRT);  
  }
  else {
    spdlog::error("Signal Handling Error");
    ::terminate(); //if failed, do termination
  }
  

  if(pthread_sigmask(SIG_SETMASK, &sigmask, nullptr)!=0){ // signal masking for this thread(main)
    spdlog::error("Signal Masking Error");
    ::terminate();
  }

  mlockall(MCL_CURRENT|MCL_FUTURE); //avoid swaping

  cxxopts::Options options(argv[0], "-  Commnad Line Options");
	options.add_options()
        ("c,config", "Load Configuration File(*.config)", cxxopts::value<std::string>(), "File Path") //require rerun avoiding
        ("i,install", "Install new RT Task", cxxopts::value<std::string>(), "RT Task")
        ("u,unintall", "Uninstall RT Task", cxxopts::value<std::string>(), "RT Task")
        ("f,force", "forced re-Run")
        ("v,version", "Openedge Service Engine Version")
        ("h,help", "Print Usage");

       
  try
  {
    auto args = options.parse(argc, argv);

    if(args.count("force")) { sem_close(_running); sem_unlink(semname); }
    
    if(args.count("version")) { cout << _OE_VER_ << endl; ::terminate(); }
    else if(args.count("install")) { cout << "Not Support yet" << endl; ::terminate(); }
    else if(args.count("uninstall")) { cout << "Not Support yet" << endl; ::terminate(); }
    else if(args.count("help")) { cout << options.help() << endl; ::terminate(); }
    else if(args.count("help")) { cout << options.help() << endl; ::terminate(); }
    else if(args.count("config")){

      //for re-run avoidance
      _running = sem_open(semname, O_CREAT | O_EXCL, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH, 1);
      if(_running==SEM_FAILED){
        spdlog::error("It cannot be allowed to re-run on same system.");
        ::terminate();
      }

      string _conf_file = args["config"].as<std::string>();

      spdlog::info("Starting Openedge Service Engine {} (built {}/{})", _OE_VER_, __DATE__, __TIME__);
      spdlog::info("Load Configuration File : {}", _conf_file);

      //run task engine
      if(oe::edge::initialize(_conf_file.c_str()))
          oe::edge::run();
      
      pause(); //wait until getting SIGINT
    }
  }
  catch(const cxxopts::OptionException& e){
    spdlog::error("Argument parse exception : {}",e.what());
  }

  ::terminate();
  return EXIT_SUCCESS;
}