/**
 * @file    instance.hpp
 * @brief   Openedge Engine Header
 */

void terminate() { 
    spdlog::info("Successfully terminated");
    exit(EXIT_SUCCESS);
}

void sig_interrupt(int param) { ::terminate(); }