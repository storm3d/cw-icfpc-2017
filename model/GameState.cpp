#include <cassert>
#include <json.hpp>

#include "GameState.h"
#include "Dijkstra.h"

// for convenience
using json = nlohmann::json;
using namespace std;

River::River(vert_t from_, vert_t to_)
{
    if (from > to) {
        from = to_;
        to = from_;
    } else {
        from = from_;
        to = to_;
    }
}

bool River::isAdjacent(const River &other) const {
    return
            from == other.from
            || to == other.to
            || to == other.from
            || from == other.to;
}

GameState::GameState() {
}

GameState::GameState(std::istream &in) {
    deserialize(in);
}

GameState::GameState(std::string json) {
    std::istringstream is(json);
    deserialize(is);
}


void GameState::serialize(std::ostream &out) const {

    out << "{\"punter\":" << punter_id << ','
        << "\"punters\":" << punters_num << ','
        << "\"map\": {";

    out     << "\"sites\": [";
    bool comma = false;
    for (vert_t v = 0; v < incidence_list.size(); v++)
    {
        if (comma) {
            out << ',';
        }
        comma = true;
        out     << "{\"id\":" << v << '}';
    }
    out     << "], "; // sites

    out     << "\"rivers\": [";
    comma = false;
    for (vert_t v1 = 0; v1 < incidence_list.size(); v1++)
    {
        for (auto& vi : incidence_list[v1]) {
            if(v1 >= vi.first)
                continue;

            if (comma) {
                out << ',';
            }
            comma = true;
            out << "{\"s\":" << v1 << ",\"t\":" << vi.first << ",\"p\":" << vi.second << '}';
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
    out << "}"; // state
}

void GameState::deserialize(std::istream &in) {
    json j;
    in >> j;
    deserialize(j);
}

void GameState::deserialize(json& state) {
    cerr << "GameState::deserialize" << endl;

    punters_num = state["punters"];
    punter_id = state["punter"];

    if(state["map"].is_object())
    {
        json &map = state["map"];
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
                vert_t source = element["s"];
                vert_t target = element["t"];
                vert_t punter = element["p"];

                incidence_list[source][target] = punter;
                incidence_list[target][source] = punter;
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

std::istream & operator << (std::istream &in, GameState& game)
{
    game.deserialize(in);
    return in;
}

vert_t GameState::getPuntersNum() const {
    return punters_num;
}

punter_t GameState::getPunterId() const {
    return punter_id;
}

vert_t GameState::getSitesNum() const {
    return incidence_list.size();
}

vert_t GameState::getMinesNum() const {
    return mines.size();
}

int GameState::getCurrentTurnNum() const {
    return currentTurn;
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

const std::unordered_map<vert_t, std::vector<vert_t>> &GameState::getMinDistances() const {
    return min_distances;
}

// complement directional edges to unidirectional
void GameState::complementEdges() {
    vert_t num = incidence_list.size();
    for (vert_t i = 0; i < num; i++) {
        for (auto& vi : incidence_list[i]) {
            vert_t puntor = vi.second;

            if (vi.first >= incidence_list.size())
                incidence_list.resize(vi.first + 1);

            incidence_list[vi.first][i] = puntor;
        }
    }
}

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
