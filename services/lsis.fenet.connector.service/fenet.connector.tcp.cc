#include "fenet.connector.service.hpp"
#include <3rdparty/spdlog/spdlog.h>

bool fenetConnectorService::connet(const char* ipv4_addr, int port){
    if(!net::tcp::async::connect(ipv4_addr, port)){
        spdlog::error("Cannot connect to the host({}:{})", ipv4_addr, port);
        return false;
    }

    return true;
}

void fenetConnectorService::on_received(){

}