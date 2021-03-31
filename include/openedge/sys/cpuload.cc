
#include "info.hpp"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <cmath>

namespace oe::sys {

void cpuload::initcpuUsage() {
    FILE *file = fopen(this->_procFile.c_str(), "r");
    auto retval = fscanf(file, "cpu %lu %lu %lu %lu", &_lastTotalUser, &_lastTotalUserLow, &_lastTotalSys, &_lastTotalIdle);
    if (retval < 0) {
        throw std::runtime_error("init cpu usage crash");
    }
    fclose(file);
}

double cpuload::getCurrentCpuUsage() {
    double percent;
    FILE *file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

    file = fopen(this->_procFile.c_str(), "r");
    auto retval = fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
                         &totalSys, &totalIdle);
    fclose(file);
    if (retval < 0) {
        throw std::runtime_error("init cpu usage crash");
    }

    if (totalUser < _lastTotalUser || totalUserLow < _lastTotalUserLow ||
        totalSys < _lastTotalSys || totalIdle < _lastTotalIdle) {
        //Overflow detection. Just skip this value.
        percent = -1.0;
    } else {
        total = (totalUser - _lastTotalUser) + (totalUserLow - _lastTotalUserLow) +
                (totalSys - _lastTotalSys);
        percent = total;
        total += (totalIdle - _lastTotalIdle);
        percent /= total;
        percent *= 100;
    }

    _lastTotalUser = totalUser;
    _lastTotalUserLow = totalUserLow;
    _lastTotalSys = totalSys;
    _lastTotalIdle = totalIdle;

    return percent;
}

std::vector<double> cpuload::getCurrentMultiCoreUsage() {

    std::vector<double> percents;
    std::ifstream file;
    file.open(this->_procFile);

    if (!file.is_open()) {
        throw std::runtime_error("unable to open " + this->_procFile);
    }

    std::string cpu;
    std::string line;

    if (this->_vec_lastTotalUser.empty() ||
        this->_vec_lastTotalUserLow.empty() ||
        this->_vec_lastTotalIdle.empty() ||
        this->_vec_lastTotalSys.empty()) {
        throw std::runtime_error("init went wrong");
    }
    uint32_t cnt = 0;
    while (std::getline(file, line)) {
        double percent;
        for (uint32_t i = cnt; i < this->_numOfCpus; i++) {
            cpu = "cpu";
            cpu += std::to_string(i);

            uint64_t totalUser = 0,
                    totalUserLow = 0,
                    totalSys = 0,
                    totalIdle = 0;
            if (line.find(cpu) != std::string::npos) {
                cpu += " %lu %lu %lu %lu";

                auto r = std::sscanf(line.c_str(), cpu.c_str(),
                                &totalUser,
                                &totalUserLow,
                                &totalSys,
                                &totalIdle);

                if (r == -1) {
                    throw std::runtime_error("fscanf of file failed init multicpu");
                }
                if (totalUser < this->_vec_lastTotalUser.at(i)
                    || totalUserLow < this->_vec_lastTotalUserLow.at(i)
                    || totalSys < this->_vec_lastTotalSys.at(i)
                    || totalIdle < this->_vec_lastTotalIdle.at(i)) {
                    percent = -1.0;
                } else {
                    uint64_t total = (totalUser - this->_vec_lastTotalUser.at(i)) +
                                     (totalUserLow - this->_vec_lastTotalUserLow.at(i)) +
                                     (totalSys - this->_vec_lastTotalSys.at(i));
                    percent = total;
                    total += (totalIdle - this->_vec_lastTotalIdle.at(i));
                    percent /= total;
                    percent *= 100;
                    if (percent < 0.0) {
                        percent = 0;
                    }
                    percent = std::round(percent * 100) / 100;
                }

                this->_vec_lastTotalSys[i] = totalSys;
                this->_vec_lastTotalIdle[i] = totalIdle;
                this->_vec_lastTotalUser[i] = totalUser;
                this->_vec_lastTotalUserLow[i] = totalUserLow;
                percents.push_back(percent);
                cnt++;
                break;
            }
        }
    }
    file.close();
    return percents;
}

void cpuload::initMultiCore() {
    std::ifstream file;
    file.open(this->_procFile);

    if (!file.is_open()) {
        throw std::runtime_error("unable to open " + this->_procFile);
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("cpu") != std::string::npos) {
            this->_numOfCpus++;
        }
    }
    this->_numOfCpus -= 1; // in the /proc/cat file there is a common cpu and the single cores cpu0 - cpuxx

    this->_vec_lastTotalSys.resize(this->_numOfCpus);
    this->_vec_lastTotalUser.resize(this->_numOfCpus);
    this->_vec_lastTotalUserLow.resize(this->_numOfCpus);
    this->_vec_lastTotalIdle.resize(this->_numOfCpus);
    std::string cpu;

    uint32_t cnt = 0;
    file.clear();
    file.seekg(0, std::ios::beg);
    while (std::getline(file, line)) {
        for (uint32_t i = cnt; i < this->_numOfCpus; i++) {
            cpu = "cpu";
            cpu += std::to_string(i);

            uint64_t totalUser, totalUserLow, totalSys, totalIdle;

            if (line.find(cpu) != std::string::npos) {
                cpu += " %lu %lu %lu %lu";
                auto r = std::sscanf(line.c_str(), cpu.c_str(),
                                &totalUser,
                                &totalUserLow,
                                &totalSys,
                                &totalIdle);
                if (r == -1) {
                    throw std::runtime_error("fscanf of file failed init multicpu");
                } else {
                    this->_vec_lastTotalSys[i] = totalSys;
                    this->_vec_lastTotalIdle[i] = totalIdle;
                    this->_vec_lastTotalUser[i] = totalUser;
                    this->_vec_lastTotalUserLow[i] = totalUserLow;
                    cnt++;
                    break;
                }
            } else {
            }
        }

    }

    file.close();
}

std::string cpuload::getCPUName(std::string cpuNameFile) {

    if(!this->_cpuName.empty()) {
        return this->_cpuName;
    }

    std::ifstream file;
    file.open(cpuNameFile);

    if (!file.is_open()) {
        throw std::runtime_error("unable to open " +cpuNameFile);
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("model name") != std::string::npos) {
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                this->_cpuName = line.substr(pos, line.size());
                return this->_cpuName;
            }
        }
    }
    return std::string();
}    

}