/**
 * @file    moxa.io.task.hpp
 * @brief   MOXA Nport Serial Device Server
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_MOXA_IO_TASK_HPP_
#define _OPENEDGE_MOXA_IO_TASK_HPP_

#include <openedge/core.hpp>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <map>
#include <string>
#include <openedge/net/udpserver.h>

namespace zmq {
    #include <czmq.h>
}

using namespace oe;
using namespace std;

class moxaIoTask : public oe::core::task::runnable {

    public:
        moxaIoTask(){};
        virtual ~moxaIoTask() = default;

        //component common interface
        bool configure() override;
        void execute() override;
        void cleanup() override;
        void pause() override;
        void resume() override;

    private: //for UDP
        int _dataport = 4002;
        long _sockfd = -1;
        long _sock_optval = 1;
        sockaddr_in _sockname;

    private: //for io
        string _devicename = "unknown";
        string _deviceip = "127.0.0.1";
        map<string, int> di_container;
        map<string, int> do_container;

};

EXPORT_TASK_API

#endif