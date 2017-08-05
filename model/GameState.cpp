#include <cassert>
#include <json.hpp>

#include "GameState.h"
#include "Dijkstra.h"

// for convenience
using json = nlohmann::json;

GameState::GameState(std::istream &in) {
    json j;
    in >> j;
    puntersNum = j["punters"];
    punterId = j["punter"];

    if(j["map"].is_object())
    {
        json &map = j["map"];
        if(map["sites"].is_array()) {
            incidence_list.resize(map["sites"].size());
            for (auto& element : map["sites"]) {
                int id = element["id"];

                incidence_list[id] = VertexIncidence();
                //std::cout << element << '\n';
            }
        }

        if(map["rivers"].is_array()) {
            for (auto& element : map["rivers"]) {
                vert_t source = element["source"];
                vert_t target = element["target"];

                incidence_list[source][target] = 0;
                incidence_list[target][source] = 0;
            }
        }

        if(map["mines"].is_array()) {
            for (auto &element : map["mines"]) {
                vert_t id = element;
                mines.insert(id);
            }
        }
    }
}

void GameState::serialize(std::ostream &out) const {

    out << "{\"punter\": " << punterId << ','
        << "\"punters\": " << puntersNum << ','
        << "\"map\": {";

    out     << "\"sites\": [";
    bool comma = false;
    for (vert_t v = 0; v < incidence_list.size(); v++)
    {
        if (comma) {
            out << ',';
        }
        comma = true;
        out     << "{\"site\": " << v << '}';
    }
    out     << "], "; // sites

    out     << "\"rivers\": [";
    comma = false;
    for (vert_t v1 = 0; v1 < incidence_list.size(); v1++)
    {
        for (vert_t v2 = 0; v2 < incidence_list.size(); v2++)
        {
            if (comma) {
                out << ',';
            }
            comma = true;
            out     << "{\"source\": " << v1 << ", \"target\": " << v2 << '}';
        }
    }
    out     << "], "; // rivers

    out     << "\"mines\": [";
    comma = false;
    for (vert_t v : mines)
    {
        if (comma) {
            out << ',';
        }
        comma = true;
        out     << v;
    }
    out     << "] "; // mines


    out << "}"; // map
    out << "}\n"; // state
}

vert_t GameState::getPuntersNum() const {
    return puntersNum;
}

punter_t GameState::getPunterId() const {
    return punterId;
}

vert_t GameState::getSitesNum() const {
    return incidence_list.size();
}

vert_t GameState::getMinesNum() const {
    return mines.size();
}

bool GameState::isMine(vert_t i) const {
    return mines.find(i) != mines.end();
}

bool GameState::isEdge(vert_t from, vert_t to) const {
    if (from > to)
    {
        vert_t tmp = from;
        from = to;
        to = tmp;
    }

    assert(to < incidence_list.size());

    auto it = incidence_list[from].find(to);
    return it != incidence_list[from].end();
}

punter_t GameState::getClaimerId(vert_t from, vert_t to) const {
    if (from > to)
    {
        vert_t tmp = from;
        from = to;
        to = tmp;
    }

    assert(to < incidence_list.size());

    auto it = incidence_list[from].find(to);
    if (it != incidence_list[from].end())
    {
        return it->second;
    }

    return 0;
}

void GameState::claimEdge(vert_t from, vert_t to, punter_t punter)  {
    if (from > to)
    {
        vert_t tmp = from;
        from = to;
        to = tmp;
    }

    assert(to < incidence_list.size());

    auto it = incidence_list[from].find(to);
    if (it != incidence_list[from].end())
    {
        incidence_list[from][to] = punter;
    }
}

const std::unordered_map<vert_t, punter_t> &GameState::getEdgesFrom(vert_t vertex) const {
    return incidence_list[vertex];
}

const std::unordered_set<vert_t>& GameState::getMines() const {
    return mines;
}

// TODO: testsssss
void GameState::initMinDistances()
{
    vert_t vertices_num = incidence_list.size();
    vert_t mines_num = mines.size();

    for (vert_t i : mines) {
        std::vector<std::vector<vert_t>> adj(getEdgesFrom(i).size());

        for (std::pair<vert_t, punter_t> v : getEdgesFrom(i)) {
            adj[i].push_back(v.first);
        }
        std::vector<vert_t> vertext_distances(vertices_num);
        Dijkstra(i, adj, vertext_distances);
        min_distances[i] = vertext_distances;
    }
}
