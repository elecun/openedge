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
#include <openedge/core/rt_trigger.hpp>

//for timer
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#define SIGTIMER (SIGRTMAX)

using namespace std;

void terminate() {
    exit(EXIT_SUCCESS);
}

void cleanup(int sig) { ::terminate(); }

// struct timespec x; 
// struct itimerspec ispec; 
// struct itimerspec ospec; 
// struct sigaction sa; 
// timer_t tid; 
timer_t timerid;
struct timespec x; 
void timer_handler(int signo, siginfo_t *info, void *context) { 
    if (signo == SIGTIMER) {
		//puts("Periodic timer");
        clock_gettime(CLOCK_REALTIME,&x); 
        printf("%ld.%09ld \n",x.tv_sec, x.tv_nsec); 
    }
    else if (signo == SIGINT) {
      timer_delete(timerid);
      perror("Ctrl + C cached!\n");
      exit(1);
    }
} 

timer_t SetTimer(int signo, int sec, int mode)
{
	struct sigevent sigev;
	timer_t timerid;
	struct itimerspec itval;
	struct itimerspec oitval;

	// Create the POSIX timer to generate signo
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = signo;
	sigev.sigev_value.sival_ptr = &timerid;

	if (timer_create(CLOCK_REALTIME, &sigev, &timerid) == 0) {
		itval.it_value.tv_sec = 0;//sec / 1000;
		itval.it_value.tv_nsec = 10*1000*1000;//(long)(sec % 1000) * (1000000L);

		if (mode == 1) {
			itval.it_interval.tv_sec = itval.it_value.tv_sec;
			itval.it_interval.tv_nsec = itval.it_value.tv_nsec;
		} else {
			itval.it_interval.tv_sec = 0;
			itval.it_interval.tv_nsec = 0;
		}

		if (timer_settime(timerid, 0, &itval, &oitval) != 0) {
			perror("time_settime error!");
		}
	} else {
		perror("timer_create error!");
		return (timer_t)-1;
	}
	return timerid;
}

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

    if(args.count("version")) { printf("%d.%d.%d\n",__MAJOR__, __MINOR__, __REV__); ::terminate(); }
    else if(args.count("config")){
      string _conf_file = args["config"].as<std::string>();
      spdlog::info("Starting Openedge Service Engine {}.{}.{} (built {}/{})", __MAJOR__, __MINOR__, __REV__, __DATE__, __TIME__);
      spdlog::info("Load Configuration File : {}", _conf_file);

      //run application
      //oe::init(_conf_file.c_str());
      //oe::run();

      //for timer
      struct sigaction sigact;
      sigemptyset(&sigact.sa_mask);
      sigact.sa_flags = SA_SIGINFO;
      sigact.sa_sigaction = timer_handler;

      if (sigaction(SIGTIMER, &sigact, NULL) == -1) {
        perror("sigaction failed");
        return -1;
      }
      // Establish a handler to catch CTRL+C and use it for exiting
      sigaction(SIGINT, &sigact, NULL);
      timerid = SetTimer(SIGTIMER, 500, 1);

    // x.tv_sec=0; 
    // x.tv_nsec=10*1000*1000; 
    // tid=1; 
    // sa.sa_sigaction = alrm_handler; 
    // sa.sa_flags = SIGEV_SIGNAL; 
    // sigaction(SIGALRM, &sa, NULL); 
    
    // timer_create(CLOCK_REALTIME,NULL,&tid); 
    // ispec.it_value.tv_sec = 0; 
    // ispec.it_value.tv_nsec = 10*1000*1000; 
    // ispec.it_interval.tv_sec = 0; 
    // ispec.it_interval.tv_nsec = 10*1000*1000;
    // timer_settime(tid, TIMER_ABSTIME, &ispec, &ospec);

      while(1)
        sleep(1);
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