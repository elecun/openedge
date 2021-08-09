
#include "test2.task.hpp"
#include <openedge/log.hpp>


//static component instance that has only single instance
static test2Task* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new test2Task(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool test2Task::configure(){
    console::info("call test2Task configure()");
    return true;
}

void test2Task::execute(){
    console::info("call test2Task execute()");
}

void test2Task::cleanup(){
    console::info("call test2Task cleanup()");
}

void test2Task::pause(){
    console::info("call test2Task pause()");
}

void test2Task::resume(){
    console::info("call test2Task resume()");
}
