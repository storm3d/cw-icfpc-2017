
#ifndef CW_ICFPC_2017_OFFLINEPROTOCOL_H
#define CW_ICFPC_2017_OFFLINEPROTOCOL_H

#include <iostream>
#include <vector>

#include <json.hpp>

#include "../model/GameState.h"

// for convenience
using json = nlohmann::json;


class OfflineProtocol {
public:

    struct Move {
        vert_t punter_id;
        vert_t from;
        vert_t to;

        Move(vert_t punter_id, vert_t from, vert_t to);
    };

    void handleRequest(std::istream &in, std::ostream &out);

    std::unique_ptr<GameState> extractStateFromSetupRequest(json &setup_request);

    std::unique_ptr<GameState> extractStateFromMoveRequest(json &setup_request);

    std::vector<Move> extractMovesFromMoveRequest(json &move_request);

    void writeSetupResponse(std::ostream &out, GameState *state);

    void writeMoveResponse(std::ostream &out, GameState *state);
};

bool operator==(const OfflineProtocol::Move& lhs, const OfflineProtocol::Move& rhs);

#endif //CW_ICFPC_2017_OFFLINEPROTOCOL_H
