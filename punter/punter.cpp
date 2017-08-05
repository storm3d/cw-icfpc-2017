#include <iostream>
#include <unistd.h>
#include <fstream>
#include "json.hpp"
#include <unistd.h>
#include <fcntl.h>

#include "../model/GameState.h"
#include "OfflineProtocol.h"

using namespace std;

string readJson()
{
    unsigned long jsonLength;
    cin >> jsonLength;

    cin.ignore(1);

    cerr << "FOUND JSON SIZE: " << jsonLength << endl;

    string jsonStr(jsonLength, ' ');
    cin.read(&jsonStr[0], jsonLength);

    return jsonStr;
}

int main( int argc, char* argv[] ) {

    const clock_t begin_time = clock();

    //std::ofstream ofs ("punter.log", std::ofstream::out);
    //ofs << "Log started" << std::endl;

    // send stub handshake
    // TODO: proper handshake
    nlohmann::json j;
    j["me"] = "cw";
    std::string handshake = j.dump();
    std::cout<<handshake.size()<<":"<<handshake;

    cerr << endl << "reading 1st json" << endl;
    string hanshake = readJson();
    cerr << endl << "reading 2nd json" << endl;
    string payload = readJson();

    cerr << endl << "generating next move" << endl;

    OfflineProtocol offlineProtocol;
    istringstream iss(payload);
    ostringstream oss;

    offlineProtocol.handleRequest(iss, oss);

    cerr << endl << "writing response: " << endl;
    cerr <<oss.str().size()<<":"<<oss.str()<<std::endl;

    std::cout<<oss.str().size()<<":"<<oss.str();
    cerr << endl << "move written! " << endl;

    std::cerr <<  std::endl << "Execution time: "
              << float( clock () - begin_time ) / CLOCKS_PER_SEC
              << " sec" << std::endl;
    return 0;
}
