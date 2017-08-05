
#include "OfflineProtocol.h"

void OfflineProtocol::handleRequest(std::istream &in, std::ostream &out) {
    json request;
    in >> request;

    if (request.find("punter") != request.end()) {
        // setup request
        std::cerr << "Handling Setup request" << std::endl;
        std::unique_ptr<GameState> state = getStateFromSetupRequest(request);

        //{"ready" : p, "state" : state}
        writeSetupResponse(out, state.get());
    }
}

std::unique_ptr<GameState> OfflineProtocol::getStateFromSetupRequest(json &setup_request) {

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

                builder.incidence_list_ref()[source][target] = 0;
                builder.incidence_list_ref()[target][source] = 0;
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

void OfflineProtocol::writeSetupResponse(std::ostream &out, GameState* state) {
    out << "{\"ready\":" << state->getPunterId() << ",\"state\":";
    state->serialize(out);
    out << "}";
}
