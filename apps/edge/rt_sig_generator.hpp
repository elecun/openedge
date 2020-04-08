
/**
 * @file    rt_sig_generator.hpp
 * @brief   Realtime Signal Generator
 * @author  Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _EDGE_RT_SIG_GENERATOR_HPP_
#define _EDGE_RT_SIG_GENERATOR_HPP_

#include <openedge/arch/singleton.hpp>
#include <map>
#include <functional>
#include <openedge/core/driver.hpp>


using namespace std;

namespace oe {
    namespace edge {

        class rt_sig_slot : public oe::arch::singleton<rt_sig_slot> {
            public:
                class sig_generator {
                    public:
                        virtual void create(long nsec) = 0;

                };

                rt_sig_slot();
                ~rt_sig_slot();

                void slot(const char* slotname);
                void attach(const char* slotname, core::task_driver* driver);

            private:
                std::map<string, sig_generator*> _bank;


        };


    } //namespace edge
} //namespace oe


#define edge_sig_generator oe::edge::rt_sig_slot::instance()

#endif