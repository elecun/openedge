/**
 * @file    agsys.manage.task.hpp
 * @brief   Application Gateway Management Task
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_AGSYS_MANAGE_TASK_HPP_
#define _OPENEDGE_AGSYS_MANAGE_TASK_HPP_

#include <openedge/core.hpp>
#include <string>

namespace zmq {
    #include <czmq.h>
}

using namespace oe;
using namespace std;

namespace oe::device {
    class controller;
    class bus;
}

class agsysManageTask : public oe::core::task::runnable  {

    public:
        agsysManageTask() = default;
        virtual ~agsysManageTask() = default;

        //component common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;
        void pause() override;
        void resume() override;

};

EXPORT_TASK_API

#endif