
// #include "instance.hpp"
// #include <external/json.hpp>
// #include <external/spdlog/spdlog.h>
// #include <fstream>
// #include <thread>

using namespace std;

namespace oe {

    //core thread callback function
    // void thread1_impl(){ 
    //     printf("test1 thread\n");
        
    //     sigset_t sigset;
    //     sigemptyset(&sigset);
    //     sigaddset(&sigset, SIGALRM); //sigalrm을 sigset에 추가
    //     sigprocmask(SIG_BLOCK, &sigset, NULL); //sigset에 있는 signal들을 block한다.

    //     pthread_sigmask(SIG_UNBLOCK, &sigset, NULL);
    // }
    
    // bool init(const char* configfile){

    //     //1. read configuration file
    //     using json = nlohmann::json;
    //     json config;
    //     try {
    //         std::ifstream file(configfile);
    //         file >> config;
    //     }
    //     catch(json::exception& e){
    //         spdlog::error("{}() throws exception ({}){}", __func__, e.id, e.what());
    //         return false;
    //     }

    //     return true;
    // }


    // void run(void){
    //     std::thread _core_thread(thread1_impl);
    //     _core_thread.join();
    // }

} //namespace oe