
#include "test1.task.hpp"
#include <openedge/log.hpp>


//static component instance that has only single instance
static test1Task* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new test1Task(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool test1Task::configure(){
    console::info("call test1Task configure()");
    return true;
}

void test1Task::execute(){
    console::info("call test1Task execute()");
}

void test1Task::cleanup(){
    console::info("call test1Task cleanup()");
}

void test1Task::pause(){
    console::info("call test1Task pause()");
}

void test1Task::resume(){
    console::info("call test1Task resume()");
}
