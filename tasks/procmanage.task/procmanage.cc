
#include "procmanage.hpp"
#include <string>
#include <signal.h>

using namespace std;

//static component instance that has only single instance
static procmanage* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new procmanage(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}



bool procmanage::configure(){

    /* default sigal container assignment */
    for(int i=(int)(SIGRTMIN+1); i<(int)(SIGRTMIN+16); i++){
        _sigrt_container.push_back(i);
    }

    /* getting configurations from profile */
    json config;
    try {
        config = json::parse(this->getProfile()->get("config"));
    } catch(json::exception& e){
        spdlog::error("process manager config error : {}", e.what());
        return false;
    }
    
    /* setup ports */
    try {
        _zmq_context = new zmq::context_t(1);
        
        /* create data port */
        if(config.find("dataport")!=config.end()){
            spdlog::info("process manage task open data port : {}", config["dataport"].get<int>());
            _zmq_dataport = new zmq::socket_t(*_zmq_context, ZMQ_PUB);
            _zmq_dataport->bind(fmt::format("tcp://*:{}", config["dataport"].get<int>()).c_str());
        }
        else {
            spdlog::warn("Process Manager has no data port. Dataport cannot be careated.");
        }

        /* create service port */
        if(config.find("serviceport")!=config.end()){
            spdlog::info("process manage task open service port : {}", config["serviceport"].get<int>());
            _zmq_serviceport = new zmq::socket_t(*_zmq_context, ZMQ_REP);
            _zmq_serviceport->bind(fmt::format("tcp://*:{}", config["serviceport"].get<int>()));
        }
    } catch(std::exception& e){
        spdlog::error("Port creation error : {}", e.what());
    }
    
    return true;
} 

void procmanage::execute(){

    /* service process */
    if(_zmq_serviceport){
        
    }

    if(_zmq_dataport){

        spdlog::info("data publishing via data port");

    // json info;
    // info["system"]["bufferram"] = _sysinfo.bufferram; /* memory used by buffers */
    // info["system"]["freehigh"] = _sysinfo.freehigh; /* available high memory size */
    // info["system"]["freeram"] = _sysinfo.freeram; /* available memory size */
    // info["system"]["load1"] = _sysinfo.loads[0];          /* 1, 5, 15 minute load average */
    // info["system"]["load5"] = _sysinfo.loads[1];          /* 1, 5, 15 minute load average */
    // info["system"]["load15"] = _sysinfo.loads[2];          /* 1, 5, 15 minute load average */
    // info["system"]["mem_unit"] = _sysinfo.mem_unit;       /* memory unit size in byte */
    // info["system"]["procs"] = _sysinfo.procs;          /* number of current processes */
    // info["system"]["sharedram"] = _sysinfo.sharedram;      /* amount of memory size */
    // info["system"]["totalhigh"] = _sysinfo.totalhigh;      /* available high memory size */
    // info["system"]["totalram"] = _sysinfo.totalram;       /* total usable main memory size */
    // info["system"]["totalswap"] = _sysinfo.totalswap;      /* total swap space size */
    // info["system"]["uptime"] = _sysinfo.uptime;         /* seconds since boot*/
    // info["system"]["totalvmem"] = (_sysinfo.totalram+_sysinfo.totalswap)*_sysinfo.mem_unit;
    // info["system"]["vmemused"] = (_sysinfo.totalram-_sysinfo.freeram+_sysinfo.totalswap-_sysinfo.freeswap)*_sysinfo.mem_unit;

    // info["network"]["rxbps"] = _netinfo.getRXBytesPerSecond();
    // info["network"]["txbps"] = _netinfo.getTXBytesPerSecond();

    // info["process"]["cpuload"] = _cpuinfo.getCurrentCpuUsage();
    
    
        // string msg = info.dump();
        // _zmq_channel_pub->send(zmq::buffer(msg), zmq::send_flags::none);
        // spdlog::info("Published processes status information");
    }
}

void procmanage::cleanup(){
    _zmq_dataport->close();
    _zmq_serviceport->close();

    delete _zmq_dataport;
    delete _zmq_serviceport;
    delete _zmq_context;
}

void procmanage::pause(){

}

void procmanage::resume(){

}

