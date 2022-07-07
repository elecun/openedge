
#include "aop.h2o2gen.emg.p2.task.hpp"
#include <openedge/log.hpp>

aop_h2o2gen_emg_p2_task::aop_h2o2gen_emg_p2_task(){


}

aop_h2o2gen_emg_p2_task::~aop_h2o2gen_emg_p2_task(){

}

void aop_h2o2gen_emg_p2_task::execute(){

    //1. read data from node
    const char* nodename = "aop_h2o2_sensor_pressure";
    double pressure = std::any_cast<double>(databus->read_from_node(nodename, 0x0000, sizeof(double)*1));

    //2. check limit (over heat warning)
    if(pressure>_limit_upper_bound){
        //alarm & system shutdown & raw water pump off
    }

}

void aop_h2o2gen_emg_p2_task::stop(){

}

bool aop_h2o2gen_emg_p2_task::configure(){

    json config = json::parse(get_profile()->get("configuration"));

    // 1. getting node configurations
    if(config.find("limit_upper_bound")!=config.end()){
        
    }

    // 2. read configurations
    if(config.find("parameters")!=config.end()){
        json parameters = config["parameters"];
        if(parameters.find("limit_upper_bound")!=parameters.end()){
            _limit_upper_bound = parameters["limit_upper_bound"].get<double>();
        }
        
    }

    _limit_upper_bound = 40.0;
    
}

void aop_h2o2gen_emg_p2_task::cleanup(){
    
}

void aop_h2o2gen_emg_p2_task::pause(){
    
}

void aop_h2o2gen_emg_p2_task::resume(){
    
}