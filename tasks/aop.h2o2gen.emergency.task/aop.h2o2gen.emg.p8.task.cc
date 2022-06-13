
#include "aop.h2o2gen.emg.p8.task.hpp"
#include <openedge/log.hpp>

aop_h2o2gen_emg_p8_task::aop_h2o2gen_emg_p8_task(){


}

aop_h2o2gen_emg_p8_task::~aop_h2o2gen_emg_p8_task(){

}

void aop_h2o2gen_emg_p8_task::execute(){

    //1. read data from node
    const char* nodename = "aop_h2o2_sensor_upw_conductivity";
    double conductivity = std::any_cast<double>(databus->read_from_node(nodename, 0x0000, sizeof(double)*1));

    //2. check limit (over heat warning)
    if(conductivity>_limit_conductivity){
        //alarm & system shutdown
    }

}

void aop_h2o2gen_emg_p8_task::stop(){

}

bool aop_h2o2gen_emg_p8_task::configure(){

    json config = json::parse(get_profile()->get("configuration"));

    // 1. getting node configurations
    if(config.find("limit_upper_bou")!=config.end()){
        
    }

    // 2. read configurations
    if(config.find("parameters")!=config.end()){
        json parameters = config["parameters"];
        if(parameters.find("limit_conductivity")!=parameters.end()){
            _limit_conductivity = parameters["limit_conductivity"].get<double>();
        }
        
    }
    
}

void aop_h2o2gen_emg_p8_task::cleanup(){
    
}

void aop_h2o2gen_emg_p8_task::pause(){
    
}

void aop_h2o2gen_emg_p8_task::resume(){
    
}
