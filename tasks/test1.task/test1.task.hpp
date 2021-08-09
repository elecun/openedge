/**
 * @file    test1.task.hpp
 * @brief   Empty task for testing
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_TEST1_TASK_HPP_
#define _OPENEDGE_TEST1_TASK_HPP_

#include <openedge/core.hpp>

using namespace oe;
using namespace std;


class test1Task : public oe::core::task::runnable  {

    public:
        test1Task() = default;
        virtual ~test1Task() = default;

        //component common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;
        void pause() override;
        void resume() override;
};

EXPORT_TASK_API

#endif