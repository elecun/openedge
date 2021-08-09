/**
 * @file    agw.manage.task.hpp
 * @brief   Application gateway management task
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_AGW_MANAGE_TASK_HPP_
#define _OPENEDGE_AGW_MANAGE_TASK_HPP_

#include <openedge/core.hpp>

using namespace oe;
using namespace std;

class agwManageTask : public oe::core::task::runnable  {

    public:
        agwManageTask() = default;
        virtual ~agwManageTask() = default;

        //component common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;
        void pause() override;
        void resume() override;
};

EXPORT_TASK_API

#endif