
/**
 * @file    info.hpp
 * @brief   system information header
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 * @ref     https://github.com/fuxey/Linux-System-Monitoring-Library.git
 */

#ifndef _OPENEDGE_SYS_INFO_HPP_
#define _OPENEDGE_SYS_INFO_HPP_

#include <sys/sysinfo.h>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <openedge/arch/singleton.hpp>

using namespace std;

namespace oe::sys {

    class memload {
        public:
            explicit memload(string memInfo = "/proc/meminfo", string memInfoOfProcess = "/proc/self/status", string memInfoOfProcessPrefix = "/proc/self/"):
                        totalMemoryInKB(0),
                        currentMemoryUsageInKB(0),
                         memInfoFile(std::move(memInfo)),
                         memInfoOfProcessFile(std::move(memInfoOfProcess)),
                         memInfoOfProcessPrefixFile(std::move(memInfoOfProcessPrefix))
                          {};
            //memory
            uint64_t getTotalMemoryInKB();
            uint64_t getCurrentMemUsageInKB();
            float getCurrentMemUsageInPercent();
            uint64_t getMemoryUsedByProcess(int pid);
            uint64_t getMemoryUsageByThisProcess();
            bool parseMemoryFile();

        private:
            uint64_t totalMemoryInKB;
            uint64_t currentMemoryUsageInKB;
            std::string memInfoFile;
            std::string memInfoOfProcessFile;
            std::string memInfoOfProcessPrefixFile;
            std::chrono::time_point<std::chrono::steady_clock> timeStamp;

    };

    class system {
        public:
            static bool isDeviceOnline(std::string address);
            static std::string getOSVersion_Signature(void);
            static std::string getOsVersionString(void);
            static std::string getIFaceMacAddress(std::string deviceName);
            static int killProcessById(int pid, const std::string& procName);
            static int getProcIdByName(const std::string& procName);
            static bool startAppAsDaemon();
            static uint64_t userAvailableFreeSpace();
            static int64_t getTemperature(const std::string& thermalZone= "thermal_zone0");
            static uint64_t getFreeDiskSpace(std::string absoluteFilePath);
            static uint64_t getSysUpTime();
            static uint32_t getNumOfThreadsByThisProcess();
            static uint32_t getNumOfThreadsByPID(int Pid);
    };

    class netload {
        public:
            explicit netload(std::string ethernetDataFileName = "/proc/net/dev", std::string ethName = "eth0");
            uint64_t getBytesPerSecond();
            uint64_t getBytesSinceStartup();
            uint64_t getRXBytesPerSecond();
            uint64_t getRXBytesSinceStartup();
            uint64_t getTXBytesPerSecond();
            uint64_t getTXBytesSinceStartup();
            static std::string getBytesPerSeceondString(uint64_t bytesPerSecond);
            static std::string getBitsPerSeceondString(uint64_t bytesPerSecond);
            static std::string getBytesString(uint64_t totalBytes);
            static std::string getBitsString(uint64_t totalBytes);
            bool isDeviceUp() const;
            std::string getDeviceName();

        private:
            void initNetworkMonitor();
            uint64_t parseEthernetDevice();
            std::string ethernetDataFile;
            std::string ethDev;
            uint64_t m_totalTransceivedBytes = 0;
            uint64_t m_totalReceivedBytes = 0;
            uint64_t m_totalTransmittedBytes = 0;
            std::map<std::string, std::string> networkstatMap;
            bool isDeviceAvailable = false;
            std::chrono::time_point<std::chrono::steady_clock> timeBefore;
            std::chrono::time_point<std::chrono::steady_clock> timeBefore_rx;
            std::chrono::time_point<std::chrono::steady_clock> timeBefore_tx;
            uint64_t BytesPerSecond = 0;
            std::chrono::time_point<std::chrono::steady_clock> timeStamp;

    };

    class cpuload {
        public:
            explicit cpuload(string procfile = "/proc/stat"):
                _procFile(std::move(procfile)), _cpuName("") {};
            void initcpuUsage();
            double getCurrentCpuUsage();
            std::vector<double> getCurrentMultiCoreUsage();
            void initMultiCore();
            std::string getCPUName(std::string cpuNameFile = "/proc/cpuinfo");
            uint32_t getCores() const { return _numOfCpus; }

        private:
            uint64_t _lastTotalUser = 0;
            uint64_t _lastTotalUserLow = 0;
            uint64_t _lastTotalSys = 0;
            uint64_t _lastTotalIdle = 0;

            vector<uint64_t> _vec_lastTotalUser;
            vector<uint64_t> _vec_lastTotalUserLow;
            vector<uint64_t> _vec_lastTotalSys;
            vector<uint64_t> _vec_lastTotalIdle;
            uint32_t _numOfCpus = 0;
            string _procFile;
            string _cpuName;
    };

    class systemInfo : public oe::arch::singleton<systemInfo>{
        public:
            systemInfo() = default;
            virtual ~systemInfo() = default;

            /**
             * @brief   obtain system resource & performance measurement
             * @return  json formatted measured data
             */
            string get_system_perform();

            /**
             * @brief   getting cpu occupy
             */
            unsigned long get_cpu_occupy(unsigned int pid);

            /**
             * 
             */
            unsigned long get_mem_occupy(unsigned int pid);
        
        private:
            struct sysinfo _sysinfo;

    }; 

} //namespace oe

#define edge_system oe::sys::systemInfo::instance()

#endif