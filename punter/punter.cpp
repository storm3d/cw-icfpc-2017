#include <iostream>
#include <unistd.h>
#include <fstream>
#include "json.hpp"
#include "../model/GameState.h"
#include "OfflineProtocol.h"

using namespace std;

string readJson()
{
    const clock_t begin_time = clock();

    char c = 0;
    string sizeStr;
    while(true)
    {
        usleep(1);
        cin >> c;
        if(c == ':')
            break;
        sizeStr += c;

        if(float( clock () - begin_time ) / CLOCKS_PER_SEC > 15)
            return "";
    }

    istringstream iss(sizeStr);
    unsigned long jsonLength;
    iss >> jsonLength;

    cerr << "FOUND JSON SIZE: " << jsonLength << endl;

    string jsonStr;

    for(int i = 0; i < jsonLength; i++)
    {
        usleep(1);
        cin >> c;
        if(cin) {
            jsonStr += c;
            cerr << c;
        }

        if(float( clock () - begin_time ) / CLOCKS_PER_SEC > 15)
            return "";
    }

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
    std::cout<<handshake.size()<<":"<<handshake<<std::endl;

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

    std::cout<<oss.str().size()<<":"<<oss.str()<<std::endl;

    cerr << endl << "move written! " << endl;

    string justWait = readJson();

/*
    nlohmann::json j2;
    j2["ready"] = "cw";
    std::string handshake2 = j2.dump();
    std::cout<<handshake2.size()<<":"<<handshake2;
*/

    //ofs.close();

    std::cerr <<  std::endl << "Execution time: "
              << float( clock () - begin_time ) / CLOCKS_PER_SEC
              << " sec" << std::endl;
    return 0;
}
