#include <iostream>
#include <fstream>
#include "json.hpp"
#include "../model/GameState.h"
#include "OfflineProtocol.h"

using namespace std;

int main( int argc, char* argv[] ) {


    const clock_t begin_time = clock();

    std::ofstream ofs ("punter.log", std::ofstream::out);
    ofs << "Log started" << std::endl;

    // send stub handshake
    // TODO: proper handshake
    nlohmann::json j;
    j["me"] = "cw";
    std::string handshake = j.dump();
    std::cout<<handshake.size()<<":"<<handshake;

    string lineInput;
    while (cin >> lineInput) {
        ofs << lineInput;
    }

    //OfflineProtocol offlineProtocol;
    //offlineProtocol.handleRequest(std::cin, std::cout);

    ofs.close();

    std::cerr <<  std::endl << "Execution time: "
              << float( clock () - begin_time ) / CLOCKS_PER_SEC
              << " sec" << std::endl;
    return 0;
}
