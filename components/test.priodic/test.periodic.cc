
#include "test.periodic.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static test_periodic* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new test_periodic(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void test_periodic::execute(){
    console::info("execute");
 
}

void test_periodic::stop(){

}

bool test_periodic::configure(){
    return true;
}

void test_periodic::cleanup(){

}

void test_periodic::pause(){
    
}

void test_periodic::resume(){
    
}

void test_periodic::on_connect(int rc){

}
void test_periodic::on_disconnect(int rc){

}
void test_periodic::on_publish(int mid){

}
void test_periodic::on_message(const struct mosquitto_message* message){

}
void test_periodic::on_subscribe(int mid, int qos_count, const int* granted_qos){

}
void test_periodic::on_unsubscribe(int mid){

}
void test_periodic::on_log(int level, const char* str){

}
void test_periodic::on_error(){
    
}

