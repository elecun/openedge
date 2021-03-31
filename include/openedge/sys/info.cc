
#include "info.hpp"
#include <openedge/core.hpp>

namespace oe::sys {

    string systemInfo::get_systeminfo(){
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
        
        return info.dump();
    }

    unsigned long systemInfo::get_cpu_occupy(unsigned int pid){

        return 0;

    }

    unsigned long systemInfo::get_mem_occupy(unsigned int pid){

        return 0;
    }

}