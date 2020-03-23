
/**
 * @file    timer.hpp
 * @brief   Timer Engine
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_CORE_TIMER_HPP_
#define _OPENEDGE_CORE_TIMER_HPP_

#include <string>
#include <thread>
#include <memory>

using namespace std;

namespace oe {
    namespace core {

        class timer {
            timer(uint64_t period_ns);
            ~timer();

            void start();   //timer start
            void stop();    //timer stop

        }; //end timer
    } //namespace core
} //namespace oe

#endif