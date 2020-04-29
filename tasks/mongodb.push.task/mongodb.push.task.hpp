
/**
 * @file    mongodb.push.task.hpp
 * @brief   Pushing data into the mongodb
 * @author  Byunghun Hwang, <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_MONGODB_PUSH_TASK_HPP_
#define _OPENEDGE_MONGODB_PUSH_TASK_HPP_

#include <openedge/core/task.hpp>
#include <openedge/core/service.hpp>
#include <map>

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
        bool loadService(const char* servicename);
        void unloadService();
};

EXPORT_RT_TASK_API

#endif