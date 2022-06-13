
#include "aop.h2o2gen.emg.p6.task.hpp"
#include <openedge/log.hpp>

aop_h2o2gen_emg_p6_task::aop_h2o2gen_emg_p6_task(){


}

aop_h2o2gen_emg_p6_task::~aop_h2o2gen_emg_p6_task(){

}

void aop_h2o2gen_emg_p6_task::execute(){

    //1. read data from node
    const char* node_vol = "aop_h2o2_sensor_voltage";
    double voltage = std::any_cast<double>(databus->read_from_node(node_vol, 0x0000, sizeof(double)*1));

    const char* node_cur = "aop_h2o2_sensor_current";
    double current = std::any_cast<double>(databus->read_from_node(node_cur, 0x0000, sizeof(double)*1));

    //2. check limit (over heat warning)
    if(voltage>=_max_voltage || current>=_max_current){
        //alarm
    }

}

void aop_h2o2gen_emg_p6_task::stop(){

}

bool aop_h2o2gen_emg_p6_task::configure(){

    json config = json::parse(get_profile()->get("configuration"));

    // 1. getting node configurations
    if(config.find("limit_upper_bound")!=config.end()){
        
    }

    // 2. read configurations
    if(config.find("parameters")!=config.end()){
        json parameters = config["parameters"];
        if(parameters.find("limit_maximum_voltage")!=parameters.end()){
            _max_voltage = parameters["limit_maximum_voltage"].get<double>();
        }

        if(parameters.find("limit_maximum_current")!=parameters.end()){
            _max_current = parameters["limit_maximum_current"].get<double>();
        }
    }
    
}

void aop_h2o2gen_emg_p6_task::cleanup(){
    
}

void aop_h2o2gen_emg_p6_task::pause(){
    
}

void aop_h2o2gen_emg_p6_task::resume(){
    
}
