
#include "sysmon.hpp"
#include <string>

using namespace std;

//static component instance that has only single instance
static sysmon* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new sysmon(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}



bool sysmon::configure(){
    _zmq_context = new zmq::context_t(1);
    _zmq_socket_pub = new zmq::socket_t(*_zmq_context, ZMQ_PUB);
    _zmq_socket_pub->bind("tcp://*:5661");

    return true;
}

void sysmon::execute(){

    sysinfo(&_sysinfo);

    json info;
    info["system"]["bufferram"] = _sysinfo.bufferram; /* memory used by buffers */
    info["system"]["freehigh"] = _sysinfo.freehigh; /* available high memory size */
    info["system"]["freeram"] = _sysinfo.freeram; /* available memory size */
    info["system"]["load1"] = _sysinfo.loads[0];          /* 1, 5, 15 minute load average */
    info["system"]["load5"] = _sysinfo.loads[1];          /* 1, 5, 15 minute load average */
    info["system"]["load15"] = _sysinfo.loads[2];          /* 1, 5, 15 minute load average */
    info["system"]["mem_unit"] = _sysinfo.mem_unit;       /* memory unit size in byte */
    info["system"]["procs"] = _sysinfo.procs;          /* number of current processes */
    info["system"]["sharedram"] = _sysinfo.sharedram;      /* amount of memory size */
    info["system"]["totalhigh"] = _sysinfo.totalhigh;      /* available high memory size */
    info["system"]["totalram"] = _sysinfo.totalram;       /* total usable main memory size */
    info["system"]["totalswap"] = _sysinfo.totalswap;      /* total swap space size */
    info["system"]["uptime"] = _sysinfo.uptime;         /* seconds since boot*/
    info["system"]["totalvmem"] = (_sysinfo.totalram+_sysinfo.totalswap)*_sysinfo.mem_unit;
    info["system"]["vmemused"] = (_sysinfo.totalram-_sysinfo.freeram+_sysinfo.totalswap-_sysinfo.freeswap)*_sysinfo.mem_unit;

    info["network"]["rxbps"] = _netinfo.getRXBytesPerSecond();
    info["network"]["txbps"] = _netinfo.getTXBytesPerSecond();

    info["process"]["cpuload"] = _cpuinfo.getCurrentCpuUsage();
    
    string msg = info.dump();
    _zmq_socket_pub->send(zmq::buffer(msg), zmq::send_flags::none);

    spdlog::info("push message");
}

void sysmon::cleanup(){
    _zmq_socket_pub->close();
    delete _zmq_socket_pub;
    delete _zmq_context;
}

void sysmon::pause(){

}

void sysmon::resume(){

}

