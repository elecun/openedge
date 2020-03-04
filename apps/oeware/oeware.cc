/**
 * @file    oeware.cc
 * @breif   OpenEdge Engine running on system background
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
#include <csignal>

#include "instance.hpp"

using namespace std;
using namespace oe;

void terminate() {
    spdlog::info("Successfully terminated");
    exit(EXIT_SUCCESS);
}
void sig_interrupt(int param) { ::terminate(); }

int main(int argc, char* argv[])
{
  signal(SIGINT, sig_interrupt);
	signal(SIGTERM, sig_interrupt);
  signal(SIGKILL, sig_interrupt);
  signal(SIGTSTP, sig_interrupt);

  spdlog::stdout_color_st("console");
  spdlog::info("Starting OEware {}.{}.{} (built {}/{})", __MAJOR__, __MINOR__, __REV__, __DATE__, __TIME__);

  cxxopts::Options options(argv[0], "");
	options.add_options()
        ("c,config", "configuration file(*.conf, *.json)", cxxopts::value<std::string>(), "FILE")
        ("h,help", "Print Usage");

  try {
    auto arguments = options.parse(argc, argv);

    if(arguments.count("config")){
      string config_file = arguments["config"].as<std::string>();
      if(oe::ware::init(config_file.c_str()))
        oe::ware::run();
      else {
        spdlog::error("OEware initialization error");
        ::terminate();
      }
      
    }

    pause();
  }
  catch(const cxxopts::OptionException& e) {
        spdlog::error("{}", e.what());
        ::terminate();
  }
    ::terminate();
    return EXIT_SUCCESS;
}