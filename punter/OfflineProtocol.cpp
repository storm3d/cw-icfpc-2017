
#include "OfflineProtocol.h"

void OfflineProtocol::handleRequest(std::istream &in, std::ostream &out) {
    json request;

    //std::cerr << "Parsing state Json" << std::endl;
    in >> request;
    //std::cerr << "Parsed state Json" << std::endl;

    if (request.find("punter") != request.end()) {
        // setup request
        std::cerr << "Handling Setup request" << std::endl;
        std::unique_ptr<GameState> state = extractStateFromSetupRequest(request);

        //{"ready" : p, "state" : state}
        writeSetupResponse(out, state.get());

    } else if (request.find("move") != request.end()) {
        // move request
        std::cerr << "Handling Move request" << std::endl;

        //out << "{\"pass\":{\"punter\":" << 0 << "},\"state\":0}";

        std::unique_ptr<GameState> state = extractStateFromMoveRequest(request);
        std::vector<Move> moves = extractMovesFromMoveRequest(request);

        for (auto& move : moves) {
            state->claimEdge(move.from, move.to, move.punter_id);
        }



        // {"claim" : {"punter" : PunterId, "source" : SiteId, "target" : SiteId}}
        // {"state" : state}
        writeMoveResponse(out, state.get());

    } else if (request.find("stop") != request.end()) {
        // stop request
        std::cerr << "Handling Stop request" << std::endl;

        std::unique_ptr<GameState> state = extractStateFromMoveRequest(request);
        std::vector<int> scores = extractScoresFromStopRequest(request);
        std::cerr << "OUR SCORE: " << scores[state->getPunterId()] << std::endl;
    }

}

std::unique_ptr<GameState> OfflineProtocol::extractStateFromSetupRequest(json &setup_request) {

    GameStateBuilder builder;

    builder.punter_id_ref() = setup_request["punter"];
    builder.punters_num_ref() = setup_request["punters"];

    if (setup_request["map"].is_object()) {
        json &map = setup_request["map"];
        if (map["sites"].is_array()) {
            builder.incidence_list_ref().resize(map["sites"].size());
            for (auto &element : map["sites"]) {
                int id = element["id"];

                builder.incidence_list_ref()[id] = VertexIncidence();
                //std::cout << element << '\n';
            }
        }

        if (map["rivers"].is_array()) {
            for (auto &element : map["rivers"]) {
                vert_t source = element["source"];
                vert_t target = element["target"];

                builder.incidence_list_ref()[source][target] = -1;
                builder.incidence_list_ref()[target][source] = -1;
            }
        }

        if (map["mines"].is_array()) {
            for (auto &element : map["mines"]) {
                vert_t id = element;
                builder.mines_ref().insert(id);
            }
        }
    }
    return builder.build();
}

std::unique_ptr<GameState> OfflineProtocol::extractStateFromMoveRequest(json &move_request)
{
    auto* state = new GameState();

    state->deserialize(move_request["state"]);

    return std::unique_ptr<GameState>(state);
}

std::vector<OfflineProtocol::Move> OfflineProtocol::extractMovesFromMoveRequest(json &move_request) {
    std::vector<OfflineProtocol::Move> moves;

    json &elements = move_request["move"]["moves"];
    if (elements.is_array()) {
        for (auto &move : elements) {
            if (move.find("claim") != move.end()) {
                OfflineProtocol::Move m(move["claim"]["punter"],
                                        move["claim"]["source"], move["claim"]["target"]);
                moves.push_back(m);
            }
        }
    }
    return moves;
}

std::vector<int> OfflineProtocol::extractScoresFromStopRequest(json &stop_request) {
    std::vector<int> scores;

    json &elements = stop_request["stop"]["scores"];
    if (elements.is_array()) {
        scores.resize(elements.size());
        for (auto &score : elements) {
            scores[score["punter"]] = score["score"];
        }
    }
    return scores;
}

void OfflineProtocol::writeSetupResponse(std::ostream &out, GameState *state) {
    out << "{\"ready\":" << state->getPunterId() << ",\"state\":";
    state->serialize(out);
    out << "}";
}

void OfflineProtocol::writeMoveResponse(std::ostream &out, GameState *state) {

    // rough algo

    // occupy mines first
    for (auto& mine : state->getMines()) {
        for (auto& edge : state->getEdgesFrom(mine)) {
            if(edge.second == -1) {

                out << "{\"claim\":{\"punter\":" << state->getPunterId() << ", \"source\":" << mine;
                out << ", \"target\":" << edge.first;
                out << "}, \"state\": ";
                state->serialize(out);
                out << "}";
                return;
            }}
    }

    // try to occupy sites
    for (auto site = 0; site < state->getSitesNum(); site++) {
        for (auto& edge : state->getEdgesFrom(site)) {
            if(edge.second == -1) {

                out << "{\"claim\":{\"punter\":" << state->getPunterId() << ", \"source\":" << site;
                out << ", \"target\":" << edge.first;
                out << "}, \"state\": ";
                state->serialize(out);
                out << "}";
                return;
            }}
    }

    out << "{\"pass\":{\"punter\":" << state->getPunterId();
    out << "}, \"state\": ";
    state->serialize(out);
    out << "}";
}

OfflineProtocol::Move::Move(vert_t punter_id, vert_t from, vert_t to) {
    this->punter_id = punter_id;
    this->from = from;
    this->to = to;
}


bool operator==(const OfflineProtocol::Move &lhs, const OfflineProtocol::Move &rhs) {
    return lhs.punter_id == rhs.punter_id
           && ((lhs.from == rhs.from && lhs.to == rhs.to)
               || (lhs.to == rhs.from && lhs.from == rhs.to));
}
