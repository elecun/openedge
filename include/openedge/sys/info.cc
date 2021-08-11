
#include "info.hpp"
#include <openedge/core.hpp>

namespace oe::sys {


    string systemInfo::get_system_perform(){

        sysinfo(&_sysinfo);

        json measure;
        measure["system"]["bufferram"] = _sysinfo.bufferram; /* memory used by buffers */
        measure["system"]["freehigh"] = _sysinfo.freehigh; /* available high memory size */
        measure["system"]["freeram"] = _sysinfo.freeram; /* available memory size */
        measure["system"]["load1"] = _sysinfo.loads[0];          /* 1, 5, 15 minute load average */
        measure["system"]["load5"] = _sysinfo.loads[1];          /* 1, 5, 15 minute load average */
        measure["system"]["load15"] = _sysinfo.loads[2];          /* 1, 5, 15 minute load average */
        measure["system"]["mem_unit"] = _sysinfo.mem_unit;       /* memory unit size in byte */
        measure["system"]["procs"] = _sysinfo.procs;          /* number of current processes */
        measure["system"]["sharedram"] = _sysinfo.sharedram;      /* amount of memory size */
        measure["system"]["totalhigh"] = _sysinfo.totalhigh;      /* available high memory size */
        measure["system"]["totalram"] = _sysinfo.totalram;       /* total usable main memory size */
        measure["system"]["totalswap"] = _sysinfo.totalswap;      /* total swap space size */
        measure["system"]["uptime"] = _sysinfo.uptime;         /* seconds since boot*/
        
        return measure.dump();
    }

    unsigned long systemInfo::get_cpu_occupy(unsigned int pid){

        return 0;

    }

    unsigned long systemInfo::get_mem_occupy(unsigned int pid){

        return 0;
    }



}