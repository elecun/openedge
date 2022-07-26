
#include "aop.uvlc.logic.hpp"
#include <openedge/log.hpp>

//static component instance that has only single instance
static aop_uvlc_logic* _instance = nullptr;
core::task::runnable* create(){ if(!_instance) _instance = new aop_uvlc_logic(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}

void aop_uvlc_logic::execute(){

    if(_l_proximity_value){
        json cmd;
        cmd["command"] = "move_cw"; //backward
        string msg = cmd.dump();
        this->publish(nullptr, _pub_topic.c_str(), strlen(msg.c_str()), msg.c_str(), 2, false); //data publish
    }
}

void aop_uvlc_logic::stop(){

}

bool aop_uvlc_logic::configure(){
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
            if(!config.contains("iomap")){
                console::error("UVLC Control IO Map configurations does not exist");
                return false;
            }

            json iomap = config["iomap"];
            console::info("IOMap : {}", iomap.dump());
            _l_proximity_in = iomap["l_proximity_in"].get<string>();
            _r_proximity_in = iomap["r_proximity_in"].get<string>();
            _wipe_forward_in = iomap["wipe_forward_in"].get<string>();
            _wipe_backward_in = iomap["wipe_backward_in"].get<string>();
            _wipe_stop_in = iomap["wipe_stop_in"].get<string>();
            _emergency_in = iomap["emergency_in"].get<string>();

            console::info("> set IO Map Left Proimity Sensor In : {}", _l_proximity_in);
            console::info("> set IO Map Right Proimity Sensor In : {}", _r_proximity_in);
            console::info("> set IO Map Wipe Forward Operation In : {}", _wipe_forward_in);
            console::info("> set IO Map Wipe Backward Operation In : {}", _wipe_backward_in);
            console::info("> set IO Map Wipe Stop In : {}", _wipe_stop_in);
            console::info("> set IO Map Emergency In : {}", _emergency_in);

            _wipe_forward_out = iomap["wipe_forward_out"].get<string>();
            _wipe_backward_out = iomap["wipe_backward_out"].get<string>();
            _wipe_stop_out = iomap["wipe_stop_out"].get<string>();
            _emergency_out = iomap["emergency_out"].get<string>();

            console::info("> set IO Map Wipe Forward Operation Out : {}", _wipe_forward_out);
            console::info("> set IO Map Wipe Backward Operation Out : {}", _wipe_backward_out);
            console::info("> set IO Map Wipe Stop Out : {}", _wipe_stop_out);
            console::info("> set IO Map Emergency Out : {}", _emergency_out);

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

void aop_uvlc_logic::cleanup(){

    /* mqtt connection close */
    this->mosqpp::mosquittopp::disconnect();
    this->mosqpp::mosquittopp::loop_stop();
    mosqpp::lib_cleanup();

}

void aop_uvlc_logic::pause(){
    
}

void aop_uvlc_logic::resume(){
    
}


void aop_uvlc_logic::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Broker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);

}
void aop_uvlc_logic::on_disconnect(int rc){

}
void aop_uvlc_logic::on_publish(int mid){

}
void aop_uvlc_logic::on_message(const struct mosquitto_message* message){

    #define MAX_BUFFER_SIZE     1024
    char* buffer = new char[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(char)*MAX_BUFFER_SIZE);
    memcpy(buffer, message->payload, sizeof(char)*message->payloadlen);
    string topic(message->topic);
    string strmsg = buffer;
    delete []buffer;

    try{
        json msg = json::parse(strmsg);
        
        if(msg.contains("di")){
            json di = msg["di"];
            if(di.contains(_l_proximity_in)) _l_proximity_value = di[_l_proximity_in].get<bool>();
        }

    }
    catch(json::exception& e){
        console::error("Message Error : {}", e.what());
    }
    console::info("mqtt data({}) : {}",message->payloadlen, strmsg);

}
void aop_uvlc_logic::on_subscribe(int mid, int qos_count, const int* granted_qos){

}
void aop_uvlc_logic::on_unsubscribe(int mid){

}
void aop_uvlc_logic::on_log(int level, const char* str){

}
void aop_uvlc_logic::on_error(){
    
}