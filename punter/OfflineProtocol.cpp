#include <queue>
#include "Solver.h"
#include "OfflineProtocol.h"

using namespace std;

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

    } else if (request.find("move") != request.end() || request.find("timeout") != request.end()) {
        // move request
        std::cerr << "Handling Move request" << std::endl;

        //out << "{\"pass\":{\"punter\":" << 0 << "},\"state\":0}";

        std::unique_ptr<GameState> state = extractCustomStateFromRequest(request);
        std::vector<Move> moves = extractMovesFromMoveRequest(request);

        for (auto& move : moves) {
            vert_t from = state->toInternalId(move.from);
            vert_t to = state->toInternalId(move.to);

            state->claimEdge(from, to, move.punter_id);
        }

        // {"claim" : {"punter" : PunterId, "source" : SiteId, "target" : SiteId}}
        // {"state" : state}

#if defined SOLVER_GREEDY
        writeMoveResponseGreedy(out, state.get());
#elif defined SOLVER_AGGREGATE
        writeMoveResponseAggregate(out, state.get());
#else
        writeMoveResponseTactic(out, state.get());
#endif

    } else if (request.find("stop") != request.end()) {
        // stop request
        std::cerr << "Handling Stop request" << std::endl;

        std::unique_ptr<GameState> state = extractCustomStateFromRequest(request);
        std::vector<int> scores = extractScoresFromStopRequest(request);

        std::cerr << endl << "SCORES: ";

        for (auto& i : scores) {
            std::cerr  << i << ", ";
        }
        std::cerr << endl << "OUR SCORE: " << scores[state->getPunterId()] << std::endl;
        std::cerr << endl << "FINISHED IN : " << state->getCurrentTurnNum() << " TURNS" << std::endl;
    }
}

std::unique_ptr<GameState> OfflineProtocol::extractStateFromSetupRequest(istream &in) {
    json request;
    in >> request;
    return extractStateFromSetupRequest(request);
}

std::unique_ptr<GameState> OfflineProtocol::extractStateFromSetupRequest(json &setup_request) {

    GameStateBuilder builder;

    builder.punter_id_ref() = setup_request["punter"];
    builder.punters_num_ref() = setup_request["punters"];

    if (setup_request.find("settings") != setup_request.end())
    {
        auto j_map_settings = setup_request["settings"];
        if(j_map_settings.find("options") != j_map_settings.end())
            builder.map_settings_ref().hasOptions = j_map_settings["options"];
        if(j_map_settings.find("futures") != j_map_settings.end())
            builder.map_settings_ref().hasFutures = j_map_settings["futures"];
        if(j_map_settings.find("splurges") != j_map_settings.end())
            builder.map_settings_ref().hasSplurges = j_map_settings["splurges"];
    }

    if (setup_request["map"].is_object()) {
        json &map = setup_request["map"];
        if (map["sites"].is_array()) {
            for (auto &element : map["sites"]) {
                int id = element["id"];

                builder.sites_ref().insert(id);
            }
        }

        if (map["rivers"].is_array()) {
            for (auto &element : map["rivers"]) {
                vert_t source = element["source"];
                vert_t target = element["target"];

                builder.add_river(source, target, -1);
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

std::unique_ptr<GameState> OfflineProtocol::extractCustomStateFromRequest(json &move_request)
{
    auto* state = new GameState();
    state->deserialize(move_request["state"]);
    return std::unique_ptr<GameState>(state);
}

std::vector<OfflineProtocol::Move> OfflineProtocol::extractMovesFromMoveRequest(json &move_request) {
    std::vector<OfflineProtocol::Move> moves;

    if(!move_request["move"].is_object())
        return moves;

    json &elements = move_request["move"]["moves"];
    if (elements.is_array()) {
        for (auto &move : elements) {
            if (move.find("claim") != move.end()) {
                OfflineProtocol::Move m(move["claim"]["punter"],
                                        move["claim"]["source"], move["claim"]["target"]);
                moves.push_back(m);
            } else if (move.find("splurge") != move.end()) {
                vert_t punter = move["splurge"]["punter"];
                bool start = true;
                vert_t src;
                for (auto &site_id :  move["splurge"]["route"]) {
                    if (start) {src = site_id; start = false; continue;};
                    int dst = site_id;
                    OfflineProtocol::Move m(punter, src, dst);
                    moves.push_back(m);
                    src = dst;
                }
            }
        }
    }
    return moves;
}

std::vector<OfflineProtocol::Option> OfflineProtocol::extractOptionsFromMoveRequest(json &move_request) {
    std::vector<OfflineProtocol::Option> options;

    json &elements = move_request["move"]["moves"];
    if (elements.is_array()) {
        for (auto &move : elements) {
            if (move.find("option") != move.end()) {
                OfflineProtocol::Option m(move["option"]["punter"],
                                        move["option"]["source"], move["claim"]["target"]);
                options.push_back(m);
            }
        }
    }
    return options;
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

void OfflineProtocol::writeMoveResponseGreedy(std::ostream &out, GameState *state) {

    // rough algo
    std::queue<punter_t> wave;
    std::unordered_set<punter_t> forbidden;

    // occupy mines first
    for (auto& mine : state->getMines()) {
        for (auto& edge : state->getEdgesFrom(mine)) {
            if(edge.second == -1) {
                writeClaimResponse(out, state, state->getPunterId(), mine, edge.first);
                return;
            }
            else if(edge.second == state->getPunterId() && forbidden.find(edge.first) == forbidden.end()) {
                wave.push(edge.first);
            }
        }
        forbidden.insert(mine);
    }

    // try to occupy sites adjacent to our already occupied
    while (!wave.empty())
    {
        int site = wave.front();
        for (auto& edge : state->getEdgesFrom(site)) {
            if (edge.second == -1) {
                writeClaimResponse(out, state, state->getPunterId(), site, edge.first);
                return;
            } else if (edge.second == state->getPunterId() && forbidden.find(edge.first) == forbidden.end()) {
                wave.push(edge.first);
            }
        }
        wave.pop();
        forbidden.insert(site);
    }

    writePassResponse(out, state, state->getPunterId());
}

void OfflineProtocol::writeMoveResponseTactic(std::ostream &out, GameState *state) {

    std::vector<PotentialEdge> edges = state->getMostPotentialEdge();

    if (edges.size() > 0) {
        writeClaimResponse(out, state, state->getPunterId(), edges[0].from, edges[0].to);
        return;
    }

    std::cerr <<  std::endl << "Panic mode started: "
              << float( clock () - GameState::punter_begin_time ) / CLOCKS_PER_SEC
              << " sec" << std::endl;

    // if we're here it means that we are landlocked - lets try to fuckup other players
    for (vert_t v1 = 0; v1 < state->getSitesNum(); v1++) {
        for (auto &vi : state->getEdgesFrom(v1)) {
            if (v1 >= vi.first)
                continue;

            if(vi.second == -1) {
                writeClaimResponse(out, state, state->getPunterId(), v1, vi.first);
                return;
            }
        }
    }

    writePassResponse(out, state, state->getPunterId());
}

void OfflineProtocol::writeMoveResponseAggregate(std::ostream &out, GameState *state) {
    if (state->getMinDistances().empty()) {
        state->initMinDistances();
    }
    Solver solver(*state);
    River r = solver.riverToClaim();

    if (r != River::EMPTY) {
        writeClaimResponse(out, state, state->getPunterId(), r.from, r.to);
    } else {
        writePassResponse(out, state, state->getPunterId());
    }
}

void OfflineProtocol::writeSetupResponse(std::ostream &out, GameState *state) {
    out << "{\"ready\":" << state->getPunterId() << ",\"state\":";
    state->serialize(out);
    out << "}";
}

void OfflineProtocol::writeClaimResponse(std::ostream &out, GameState *state, punter_t punterId, vert_t source, vert_t dist) {
    std::cerr <<  std::endl << "Writing claim response started: "
              << float( clock () - GameState::punter_begin_time ) / CLOCKS_PER_SEC
              << " sec" << std::endl;

    out << "{\"claim\":{\"punter\":" << punterId;
    out << ", \"source\":" << state->toExternalId(source);
    out << ", \"target\":" << state->toExternalId(dist);
    out << "}, \"state\": ";
    state->serialize(out);
    out << "}";
}

void OfflineProtocol::writePassResponse(std::ostream &out, GameState *state, punter_t punterId) {
    out << "{\"pass\":{\"punter\":" << punterId;
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
