
#include "sysmon.hpp"

//static component instance that has only single instance
static sysmon* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new sysmon(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}



bool sysmon::configure(){
    spdlog::info("create pull");
    // _tcp_push = zmq::zsock_new_push("@tcp://0.0.0.0:5560");

    //zmq::context_t context(1); // zmq thread pool, 0 if use only inproc, others >=1
    //zmq::socket_t socket(context, ZMQ_PUSH);

    _zmq_context = new zmq::context_t(2);
    _zmq_push_socket = new zmq::socket_t(*_zmq_context, zmq::socket_type::push);
    _zmq_push_socket->bind("tcp://0.0.0.0:5555");

    return true;
}

void sysmon::execute(){

    const std::string_view m = "Hello, world";
    _zmq_push_socket->send(zmq::buffer(m), zmq::send_flags::dontwait);
    spdlog::info("push message");
}

void sysmon::cleanup(){
    _zmq_push_socket->close();
    delete _zmq_push_socket;
    delete _zmq_context;
}

void sysmon::pause(){

}

void sysmon::resume(){

}