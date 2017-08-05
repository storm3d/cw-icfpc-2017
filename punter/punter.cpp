#include <iostream>
#include "json.hpp"
#include "../model/GameState.h"
#include "OfflineProtocol.h"

int main( int argc, char* argv[] ) {

    // send stub handshake
    // TODO: proper handshake
    nlohmann::json j;
    j["me"] = "cw";
    std::string handshake = j.dump();
    std::cout<<handshake.size()<<":"<<handshake;

    const clock_t begin_time = clock();

    // skip message prefix
    // TODO: proper reading length of JSON
    char ch;
    unsigned long len;
    std::cin>>len>>ch;
    for(int i=len; i-->0;) std::cin>>ch; // skip handshake
    std::cin>>len>>ch;

    OfflineProtocol offlineProtocol;

    offlineProtocol.handleRequest(std::cin, std::cout);

    std::cerr <<  std::endl << "Execution time: "
              << float( clock () - begin_time ) / CLOCKS_PER_SEC
              << " sec" << std::endl;

    return 0;
}
