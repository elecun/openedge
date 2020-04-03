/**
 * @file    edge.cc
 * @brief   OpenEdge Service Engine
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

#include <external/cxxopts.hpp>
#include <external/spdlog/spdlog.h>
#include <external/spdlog/sinks/stdout_color_sinks.h>
#include <sys/mman.h>
#include <openedge/core/rt_timer.hpp>
#include <openedge/core/version.hpp>

//for timer
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#define SIGTIMER (SIGRTMAX)

using namespace std;

void terminate() {
    exit(EXIT_SUCCESS);
}

void cleanup(int sig) { ::terminate(); }

class thread1 : public oe::core::rt_timer {
  public:
    void call(){
      clock_gettime(CLOCK_REALTIME,&x); 
      printf("t2 : %ld.%09ld \n",x.tv_sec, x.tv_nsec); 
    }
  private:
    struct timespec x; 
};

class thread2 : public oe::core::rt_timer {
  public:
    void call(){
      clock_gettime(CLOCK_REALTIME,&x); 
      printf("t1 : %ld.%09ld \n",x.tv_sec, x.tv_nsec); 
    }
  private:
    struct timespec x; 
};

int main(int argc, char* argv[])
{
  signal(SIGINT, cleanup);
	signal(SIGTERM, cleanup); //pkill, kill (kill -9 cannot handle)
  signal(SIGKILL, cleanup);
  signal(SIGTSTP, cleanup);
  signal(SIGABRT, cleanup); //aborted core dump

  mlockall(MCL_CURRENT|MCL_FUTURE); //avoid swaping

  spdlog::stdout_color_st("console");

  cxxopts::Options options(argv[0], "");
	options.add_options()
        ("c,config", "Load Configuration File(*.json)", cxxopts::value<std::string>(), "FILE")
        ("v,version", "Openedge Service Engine Version")
        ("h,help", "Print Usage");

  try 
  {
    auto args = options.parse(argc, argv);

    if(args.count("version")) { printf("%s\n", _OE_VER_); ::terminate(); }
    else if(args.count("config")){
      string _conf_file = args["config"].as<std::string>();
      spdlog::info("Starting Openedge Service Engine {} (built {}/{})", _OE_VER_, __DATE__, __TIME__);
      spdlog::info("Load Configuration File : {}", _conf_file);

      //for test
      thread1 t1;
      thread2 t2;
      t1.start(10*1000*1000);
      t2.start(100*1000*1000);
      
      while(1)
        sleep(1);

      t1.stop();
      t2.stop();
      //pause();  //wait until terminate signal  
      
    }

  }
  catch(const cxxopts::OptionException& e){
    spdlog::error("{}", e.what());
  }

  spdlog::info("Sucessfully Terminated");
  ::terminate();

  return EXIT_SUCCESS;
}