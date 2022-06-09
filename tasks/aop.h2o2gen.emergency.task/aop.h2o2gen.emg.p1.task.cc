
#include "aop.h2o2gen.emg.p1.task.hpp"
#include <openedge/log.hpp>

aop_h2o2gen_emg_p1_task::aop_h2o2gen_emg_p1_task(){


}

aop_h2o2gen_emg_p1_task::~aop_h2o2gen_emg_p1_task(){

}

void aop_h2o2gen_emg_p1_task::execute(){

    // 1. read temperature from other component's shared on system
    json a = this->ipc.read("")
    
    
}

void aop_h2o2gen_emg_p1_task::stop(){

}

bool aop_h2o2gen_emg_p1_task::configure(){
    return true;
}

void aop_h2o2gen_emg_p1_task::cleanup(){
    
}

void aop_h2o2gen_emg_p1_task::pause(){
    
}

void aop_h2o2gen_emg_p1_task::resume(){
    
}
