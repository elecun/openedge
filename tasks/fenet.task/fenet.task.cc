

#include "fenet.task.hpp"
#include <openedge/log.hpp>
// #include "fenetservice.hpp" //fast ethernet service


//static component instance that has only single instance
static fenetTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new fenetTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}


bool fenetTask::configure(){

    //initialize mosquitto
    if(const int ret = mosqpp::lib_init()!=MOSQ_ERR_SUCCESS){
        console::error("({}){}", ret, mosqpp::strerror(ret));
        return false;
    }

    //read configuration from profile
    json config = json::parse(getProfile()->get("configurations"));

    //read MQTT parameters & connect to the broker
    if(config.find("mqtt")!=config.end()){
        json mqtt_param = config["mqtt"];
        if(mqtt_param.find("broker")!=mqtt_param.end()) _mqtt_broker = mqtt_param["broker"].get<string>();
        if(mqtt_param.find("port")!=mqtt_param.end()) _mqtt_port = mqtt_param["port"].get<int>();
        if(mqtt_param.find("pub_topic")!=mqtt_param.end()) _mqtt_pub_topic = mqtt_param["pub_topic"].get<string>();
        if(mqtt_param.find("pub_qos")!=mqtt_param.end()) _mqtt_pub_qos = mqtt_param["pub_qos"].get<int>();
        if(mqtt_param.find("keep_alive")!=mqtt_param.end()) _mqtt_keep_alive = mqtt_param["keep_alive"].get<int>();
        if(mqtt_param.find("sub_topic")!=mqtt_param.end()){
            for(json::iterator itr=mqtt_param["sub_topic"].begin(); itr!=mqtt_param["sub_topic"].end(); ++itr){
                _mqtt_sub_topics.emplace_back(*itr);
            }
        } 

        console::info("> set MQTT Broker : {}", _mqtt_broker);
        console::info("> set MQTT Port : {}", _mqtt_port);
        console::info("> set MQTT Pub. Topic : {}", _mqtt_pub_topic);
        console::info("> set MQTT Pub. QoS : {}", _mqtt_pub_qos);
        console::info("> set MQTT Keep-alive : {}", _mqtt_keep_alive);

        //connect to MQTT broker
        if(const int conret = this->connect_async(_mqtt_broker.c_str(), _mqtt_port, _mqtt_keep_alive)==MOSQ_ERR_SUCCESS){
            for(string topic:_mqtt_sub_topics){
                this->subscribe(nullptr, topic.c_str(), 2);
                console::info("> set MQTT Sub. Topic : {}", topic);
            }
                
            if(const int ret = this->loop_start()!=MOSQ_ERR_SUCCESS)
                console::warn("({}){}", ret, mosqpp::strerror(ret));
        }
        else
            console::warn("({}){}", conret, mosqpp::strerror(conret));
    } /* config for MQTT */

    if(config.find("fenet")!=config.end()){
        json fenet = config["fenet"];
        if(fenet.find("parameter")!=fenet.end()){
            json fenet_param = fenet["parameter"];
            
            if(fenet_param.find("target_address")!=fenet_param.end()) _target_address = fenet_param["target_address"].get<string>();
            if(fenet_param.find("target_port")!=fenet_param.end()) _target_port = fenet_param["target_port"].get<int>();

            console::info("> set FENET Address : {}", _target_address);
            console::info("> set FENET Port : {}", _target_port);
        }

        //create FENet Instance
        // _service = make_unique<oe::core::service>(new oe::support::FENetService(_fenet_address.c_str(), _fenet_port));
    }

    return true;
}

void fenetTask::execute(){
    // if(_service->valid()){
    //     using oe::support::FENetService;
    //     FENetService* impl = dynamic_cast<FENetService*>(_service.get());
    //     vector<unsigned char> raw = impl->block_read();
    // }

    // _fenet->request();

}


void fenetTask::cleanup(){

    //MQTT connection close
    this->disconnect();
    this->loop_stop();
    mosqpp::lib_cleanup();
}

void fenetTask::pause(){

}

void fenetTask::resume(){

}


void fenetTask::on_connect(int rc){
    if(rc==MOSQ_ERR_SUCCESS)
        console::info("Successfully connected to MQTT Brocker({})", rc);
    else
        console::warn("MQTT Broker connection error : {}", rc);
}

void fenetTask::on_disconnect(int rc){

}

void fenetTask::on_publish(int mid){
    
}

void fenetTask::on_message(const struct mosquitto_message* message){
    
}

void fenetTask::on_subscribe(int mid, int qos_count, const int* granted_qos){
    
}

void fenetTask::on_unsubscribe(int mid){

}

void fenetTask::on_log(int level, const char* str){

}

void fenetTask::on_error(){

}

