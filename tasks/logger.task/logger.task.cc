

#include "logger.task.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <ctime>

extern "C" 
{
    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>
}

using namespace std;

//static component instance that has only single instance
static loggerTask* _instance = nullptr;
oe::core::task::runnable* create(){ if(!_instance) _instance = new loggerTask(); return _instance; }
void release(){ if(_instance){ delete _instance; _instance = nullptr; }}



bool loggerTask::configure(){

    json prepherals = json::parse(getProfile()->get("prepherals"));
    string port = prepherals["port"].get<std::string>();
    spdlog::info("device port : {}", port);

    _fd = ::open(port.c_str(), O_RDWR);
    if(_fd<0)
        return false;

    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    string filename = fmt::format("{}-{}-{}-{}-{}-{}.txt", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    _logfile.open(filename.c_str());
    _logfile << "40(ch1),40(ch2),40(ch3),41(ch1),41(ch2),41(ch3)\n";

    spdlog::info("logfile : {}", filename);

    return true;
}

void loggerTask::execute(){

    if(_fd>0){
        if(ioctl(_fd, I2C_SLAVE, 0x40)>=0){
            unsigned short value[3] = {0x0000, };
            value[0] = (unsigned short)i2c_smbus_read_word_data(_fd, 0x02);
            value[1] = (unsigned short)i2c_smbus_read_word_data(_fd, 0x04);
            value[2] = (unsigned short)i2c_smbus_read_word_data(_fd, 0x06);
            _logfile << fmt::format("{0:d},{1:d},{2:d},", (unsigned short)(value[0]>>8|value[0]<<8),
                                                        (unsigned short)(value[1]>>8|value[1]<<8),
                                                        (unsigned short)(value[2]>>8|value[2]<<8));
            //spdlog::info("40 Value : {0:x}, {1:x}, {2:x}", value[0], value[1], value[2]);
        }
        else
            spdlog::warn("cannot control i2c bus");

        if(ioctl(_fd, I2C_SLAVE, 0x41)>=0){
            unsigned short value[3] = {0x0000, };
            value[0] = (unsigned short)i2c_smbus_read_word_data(_fd, 0x02);
            value[1] = (unsigned short)i2c_smbus_read_word_data(_fd, 0x04);
            value[2] = (unsigned short)i2c_smbus_read_word_data(_fd, 0x06);
            _logfile << fmt::format("{0:d},{1:d},{2:d}", (unsigned short)(value[0]>>8|value[0]<<8),
                                                        (unsigned short)(value[1]>>8|value[1]<<8),
                                                        (unsigned short)(value[2]>>8|value[2]<<8));
            //spdlog::info("41 Value : {0:x}, {1:x}, {2:x}", value[0], value[1], value[2]);
        }
        else
            spdlog::warn("cannot control i2c bus");
        
        _logfile << "\n";
    }
    else {
        spdlog::warn("cannot open device file");
    }
}

void loggerTask::cleanup(){
    if(_fd>0)
        ::close(_fd);

    if(_logfile.is_open())
        _logfile.close();
}
