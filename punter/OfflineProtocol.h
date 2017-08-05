
#ifndef CW_ICFPC_2017_OFFLINEPROTOCOL_H
#define CW_ICFPC_2017_OFFLINEPROTOCOL_H

#include <iostream>

#include <json.hpp>

#include "../model/GameState.h"

// for convenience
using json = nlohmann::json;


class OfflineProtocol {
public:
    void handleRequest(std::istream &in, std::ostream &out);

    std::unique_ptr<GameState> getStateFromSetupRequest(json &setup_request);

    void writeSetupResponse(std::ostream &out, GameState* state);
};


#endif //CW_ICFPC_2017_OFFLINEPROTOCOL_H
