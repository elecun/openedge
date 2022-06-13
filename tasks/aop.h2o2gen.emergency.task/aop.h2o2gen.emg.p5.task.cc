
#include "aop.h2o2gen.emg.p5.task.hpp"
#include <openedge/log.hpp>

aop_h2o2gen_emg_p5_task::aop_h2o2gen_emg_p5_task(){


}

aop_h2o2gen_emg_p5_task::~aop_h2o2gen_emg_p5_task(){

}

void aop_h2o2gen_emg_p5_task::execute(){

    //1. read data from node
    const char* node_vol = "aop_h2o2_sensor_voltage";
    double voltage = std::any_cast<double>(databus->read_from_node(node_vol, 0x0000, sizeof(double)*1));

    const char* node_cur = "aop_h2o2_sensor_current";
    double current = std::any_cast<double>(databus->read_from_node(node_cur, 0x0000, sizeof(double)*1));

    //2. check limit (over heat warning)
    if(voltage<=_min_voltage || current<=_min_current){
        //alarm
    }

}

void aop_h2o2gen_emg_p5_task::stop(){

}

bool aop_h2o2gen_emg_p5_task::configure(){

    json config = json::parse(get_profile()->get("configuration"));

    // 1. getting node configurations
    if(config.find("limit_upper_bound")!=config.end()){
        
    }

    // 2. read configurations
    if(config.find("parameters")!=config.end()){
        json parameters = config["parameters"];
        if(parameters.find("limit_minimum_voltage")!=parameters.end()){
            _min_voltage = parameters["limit_minimum_voltage"].get<double>();
        }

        if(parameters.find("limit_minimum_current")!=parameters.end()){
            _min_current = parameters["limit_minimum_current"].get<double>();
        }
    }
    
}

void aop_h2o2gen_emg_p5_task::cleanup(){
    
}

void aop_h2o2gen_emg_p5_task::pause(){
    
}

void aop_h2o2gen_emg_p5_task::resume(){
    
}
