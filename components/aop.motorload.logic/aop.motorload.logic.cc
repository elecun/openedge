
#include "aop.motorload.logic.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static aop_motorload_logic* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new aop_motorload_logic(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void aop_motorload_logic::execute(){

    if(is_over_current()){
        console::info("Over current is detected! - Mean({}<{})", _mean_value, _upper_bound);
        move_stop();
    }
    else {
        console::info("Over current is not detected - Mean({}>={})", _mean_value, _upper_bound);
    }
}

void aop_motorload_logic::stop(){

}

bool aop_motorload_logic::configure(){
    try {
        const json& profile = this->get_profile()->raw();

        if(profile.contains(PROFILE_CONFIGURATIONS_KEY)){
            json config = profile[PROFILE_CONFIGURATIONS_KEY];

            /* MQTT configurations */
            if(!config.contains("mqtt")){
                console::error("MQTT configurations does not exist");
                return false;
            }

            json mqtt_param = config["mqtt"];
            _broker_address = mqtt_param["broker"].get<string>();
            _broker_port = mqtt_param["port"].get<int>();
            _pub_topic = mqtt_param["pub_topic"].get<string>();
            _pub_qos = mqtt_param["pub_qos"].get<int>();
            _keep_alive = mqtt_param["keep_alive"].get<int>();
            if(mqtt_param.find("sub_topic")!=mqtt_param.end()){
                for(json::iterator itr=mqtt_param["sub_topic"].begin(); itr!=mqtt_param["sub_topic"].end(); ++itr){
                    _sub_topics.emplace_back(*itr);
                }
            }

            console::info("> set MQTT Broker : {}", _broker_address);
            console::info("> set MQTT Port : {}", _broker_port);
            console::info("> set MQTT Pub. Topic : {}", _pub_topic);
            console::info("> set MQTT Pub. QoS : {}", _pub_qos);
            console::info("> set MQTT Keep-alive : {}", _keep_alive);

            //connect to MQTT broker
            if(const int conret = this->connect_async(_broker_address.c_str(), _broker_port, _keep_alive)==MOSQ_ERR_SUCCESS){
                for(string topic:_sub_topics){
                    this->subscribe(nullptr, topic.c_str(), 2);
                    console::info("> Subscribe topic : {}", topic);
                }
                    
                if(const int ret = this->loop_start()!=MOSQ_ERR_SUCCESS)
                    console::warn("({}){}", ret, mosqpp::strerror(ret));
            }
            else
                console::warn("({}){}", conret, mosqpp::strerror(conret));

            /* logic parameters configurations */
            if(!config.contains("boundary")){
                console::error("UVLC Motor Current Boundary does not defined.");
                return false;
            }

            json bound = config["boundary"];
            console::info("Current Sensor Bound : {}", bound.dump());
            _lower_bound = bound["lower_bound"].get<double>();
            _upper_bound = bound["upper_bound"].get<double>();
            _mean_filter = bound["mean_filter"].get<int>();
            _aio_name = bound["aio"].get<string>();

            console::info("> set Motor Current Lower Bound : {}", _lower_bound);
            console::info("> set Motor Current Upper Bound : {}", _upper_bound);
            console::info("> set Current Mean Filter size : {}", _mean_filter);
            console::info("> set Target AIO Name : {}", _aio_name);

            /* set initial value */
            _mean_value = _lower_bound;
        }
        else
            return false;
    }
    catch(const json::exception& e){
        console::error("Profile read/access error : {}", e.what());
        return false;
    }

    return true;
}

void aop_motorload_logic::cleanup(){

    /* mqtt connection close */
    this->mosqpp::mosquittopp::disconnect();
    this->mosqpp::mosquittopp::loop_stop();
    mosqpp::lib_cleanup();

}

void aop_motorload_logic::pause(){
    
}

void aop_motorload_logic::resume(){
    
}


void aop_motorload_logic::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Broker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);

}
void aop_motorload_logic::on_disconnect(int rc){

}
void aop_motorload_logic::on_publish(int mid){

}
void aop_motorload_logic::on_message(const struct mosquitto_message* message){

    #define MAX_BUFFER_SIZE     1024
    char* buffer = new char[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(char)*MAX_BUFFER_SIZE);
    memcpy(buffer, message->payload, sizeof(char)*message->payloadlen);
    string topic(message->topic);
    string strmsg = buffer;
    delete []buffer;

    try{
        json msg = json::parse(strmsg);
        
        if(msg.contains("ai")){
            json ai = msg["ai"];
            if(ai.contains(_aio_name)){
                double value = ai[_aio_name].get<double>();
                _ai_buffer.push_back(value);
                
                if(_ai_buffer.size()>=_mean_filter){
                    double sum = 0;
                    for(double v:_ai_buffer)
                        sum+=v;
                    _mean_value = sum/(double)_mean_filter;
                    _ai_buffer.pop_front();
                }
            }
        }
    }
    catch(json::exception& e){
        console::error("Message Error : {}", e.what());
    }

}
void aop_motorload_logic::on_subscribe(int mid, int qos_count, const int* granted_qos){

}
void aop_motorload_logic::on_unsubscribe(int mid){

}
void aop_motorload_logic::on_log(int level, const char* str){

}
void aop_motorload_logic::on_error(){
    
}

bool aop_motorload_logic::is_over_current(){

    if(_mean_value>_upper_bound){
        console::info("Detect Over Current!");
        return true;
    }
    
    return false;
}

void aop_motorload_logic::move_stop(){
    json cmd;
    cmd["command"] = "stop"; //forward
    string msg = cmd.dump();
    this->publish(nullptr, _pub_topic.c_str(), strlen(msg.c_str()), msg.c_str(), 2, false); //data publish
}
