

/**
 * @file    mongodb.push.task
 * @brief   mongo DB Data Push Task
 * @author  Byunghun Hwaang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_MONGODB_PUSH_TASK_HPP_
#define _OPENEDGE_MONGODB_PUSH_TASK_HPP_

#include <openedge/core/task.hpp>

using namespace oe;

class mongodbPushTask : public core::task::runnable {
    public:
        mongodbPushTask() = default;
        virtual ~mongodbPushTask() = default;

        //common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;

    private:
        
};

EXPORT_RT_TASK_API


#endif