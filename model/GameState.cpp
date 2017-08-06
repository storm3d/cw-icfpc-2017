#include <cassert>
#include <climits>
#include <queue>
#include <set>
#include "GameState.h"

// for convenience
using json = nlohmann::json;
using namespace std;

const float MINE_POTENTIAL = 100;

const River River::EMPTY = River(0, 0);

River::River(vert_t from_, vert_t to_) {
    if (from_ > to_) {
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

bool River::operator==(const River &rhs) const {
    return from == rhs.from && to == rhs.to;
}

bool River::operator!=(const River &rhs) const {
    return !(rhs == *this);
}

bool River::contains(vert_t v) const {
    return from == v || to == v;
}

GameState::GameState() : currentTurn(0) {
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
        << "\"turn\":" << currentTurn << ','
        << "\"punters\":" << punters_num << ','
        << "\"settings\":{\"f\":" << (map_settings.hasFutures?1:0) << ",\"s\":" << (map_settings.hasSplurges?1:0)<< ",\"o\":" << (map_settings.hasOptions?1:0)<<"},"
        << "\"map\": {";

    out << "\"sites\": [";
    bool comma = false;
    for (vert_t v = 0; v < incidence_list.size(); v++) {
        if (comma) {
            out << ',';
        }
        comma = true;
        out << "{\"id\":" << v << '}';
    }
    out << "], "; // sites

    out << "\"rivers\": [";
    comma = false;
    for (vert_t v1 = 0; v1 < incidence_list.size(); v1++) {
        for (auto &vi : incidence_list[v1]) {
            if (v1 >= vi.first)
                continue;

            if (comma) {
                out << ',';
            }
            comma = true;
            out << "{\"s\":" << v1 << ",\"t\":" << vi.first << ",\"p\":" << vi.second << '}';
        }
    }
    out << "], "; // rivers

    out << "\"mines\": [";
    comma = false;
    for (vert_t v : mines) {
        if (comma) {
            out << ',';
        }
        comma = true;
        out << v;
    }
    out << "] "; // mines

    out << "}"; // map

    out << ",\"remap_ids\":" << remap_ids;
    if (remap_ids) {
        out << ",\"external_ids\":[";
        comma = false;
        for (vert_t id : internal_to_external_id_mapping) {
            if (comma) out << ',';
            out << id;
            comma = true;
        }
        out << ']';
    }
    out << "}"; // state
}

void GameState::deserialize(std::istream &in) {
    json j;
    in >> j;
    deserialize(j);
}

void GameState::deserialize(nlohmann::json &state) {
    //cerr << "GameState::deserialize" << endl;

    punters_num = state["punters"];
    punter_id = state["punter"];
    currentTurn = state["turn"];
    map_settings.hasFutures = 1 == state["settings"]["f"];
    map_settings.hasOptions = 1 == state["settings"]["o"];
    map_settings.hasSplurges = 1 == state["settings"]["s"];
    currentTurn++;

    if (state["map"].is_object()) {
        json &map = state["map"];
        if (map["sites"].is_array()) {
            incidence_list.resize(map["sites"].size());
            for (auto &element : map["sites"]) {
                int id = element["id"];

                incidence_list[id] = VertexIncidence();
            }
        }

        if (map["rivers"].is_array()) {
            for (auto &element : map["rivers"]) {
                vert_t source = element["s"];
                vert_t target = element["t"];
                vert_t punter = element["p"];

                incidence_list[source][target] = punter;
                incidence_list[target][source] = punter;
            }
        }

        if (map["mines"].is_array()) {
            for (auto &element : map["mines"]) {
                vert_t id = element;
                mines.insert(id);
            }
        }

    }

    remap_ids = state["remap_ids"] != 0;
    if (remap_ids) {
        vert_t internal_id = 0;
        for (auto &element : state["external_ids"]) {
            vert_t external_id = element;
            internal_to_external_id_mapping.push_back(external_id);
            external_to_internal_id_mapping[external_id] = internal_id;
            internal_id++;
        }
    }

    incidence_available = incidence_list;
}

std::istream &operator<<(std::istream &in, GameState &game) {
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
    if (from > to) {
        vert_t tmp = from;
        from = to;
        to = tmp;
    }

    assert(to < incidence_list.size());

    auto it = incidence_list[from].find(to);
    return it != incidence_list[from].end();
}

punter_t GameState::getClaimerId(vert_t from, vert_t to) const {
    if (from > to) {
        vert_t tmp = from;
        from = to;
        to = tmp;
    }

    assert(to < incidence_list.size());

    auto it = incidence_list[from].find(to);
    if (it != incidence_list[from].end()) {
        return it->second;
    }

    return -1;
}

void GameState::claimEdge(vert_t from, vert_t to, punter_t punter) {
    if (from > to) {
        vert_t tmp = from;
        from = to;
        to = tmp;
    }

    if (punter == punter_id) {
        our_sites.insert(from);
        our_sites.insert(to);
    }

    assert(to < incidence_list.size());

    auto it = incidence_list[from].find(to);
    if (it != incidence_list[from].end()) {
        incidence_list[from][to] = punter;
        incidence_available[from].erase(to);
    }

    it = incidence_list[to].find(from);
    if (it != incidence_list[to].end()) {
        incidence_list[to][from] = punter;
        incidence_available[to].erase(from);
    }
}

const std::unordered_map<vert_t, punter_t> &GameState::getEdgesFrom(vert_t vertex) const {
    return incidence_list[vertex];
}

const std::unordered_map<vert_t, punter_t> &GameState::getAvailableEdgesFrom(vert_t vertex) const {
    return incidence_available[vertex];
}

const std::unordered_set<vert_t> &GameState::getMines() const {
    return mines;
}

const std::unordered_map<vert_t, std::vector<vert_t>> &GameState::getMinDistances() const {
    return min_distances;
}

/*
// complement directional edges to unidirectional
void GameState::complementEdges() {
    vert_t num = incidence_list.size();
    for (vert_t i = 0; i < num; i++) {
        for (auto& vi : incidence_list[i]) {
            punter_t puntor = vi.second;

            if (vi.first >= incidence_list.size())
                incidence_list.resize(vi.first + 1);

            //cerr << "comp " << incidence_list[vi.first][i] << " " << puntor << endl;

            incidence_list[vi.first][i] = puntor;
        }
    }
}
 */

void GameState::initMinDistances() {
    vert_t vertices_num = incidence_list.size();
    vert_t mines_num = mines.size();

    for (vert_t v = 0; v < vertices_num; v++) {
        min_distances[v].resize(vertices_num, INT_MAX);

        for (auto continuation : incidence_list[v]) {
            vert_t v2 = continuation.first;
            min_distances[v2].resize(vertices_num, INT_MAX);

            min_distances[v][v2] = 1;
            min_distances[v2][v] = 1;
        }
        min_distances[v][v] = 0;
    }

    // Variant of Dijkstra: https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm
    for (vert_t intermVertex = 0; intermVertex < vertices_num; intermVertex++) {
//        for (vert_t mine : mines) { - doesn't work somewhy.
        for (vert_t startVertex = 0; startVertex < vertices_num; startVertex++) {
            for (vert_t endVertex = 0; endVertex < vertices_num; endVertex++) {
                if (isEdge(startVertex, intermVertex) && isEdge(intermVertex, endVertex)) {
                    vert_t old_d = min_distances[startVertex][endVertex];
                    vert_t new_d = min_distances[startVertex][intermVertex] + min_distances[intermVertex][endVertex];
                    min_distances[startVertex][endVertex] = std::min(new_d, old_d);
                }
            }
        }
    }
}

void GameStateBuilder::add_river(vert_t from, vert_t to, punter_t punter) {
    if (incidence_map.find(from) == incidence_map.end())
        incidence_map[from] = VertexIncidence();

    incidence_map[from][to] = punter;

    if (incidence_map.find(to) == incidence_map.end())
        incidence_map[to] = VertexIncidence();

    incidence_map[to][from] = punter;
}

const vert_t GameState::toInternalId(vert_t externalId) {
    return remap_ids ? external_to_internal_id_mapping[externalId] : externalId;
}

const vert_t GameState::toExternalId(vert_t internalId) {
    return remap_ids ? internal_to_external_id_mapping[internalId] : internalId;
}


std::unique_ptr<GameState> GameStateBuilder::build() {
    auto *state = new GameState();

    state->punter_id = punter_id;
    state->punters_num = punters_num;
    state->map_settings = map_settings;

    state->remap_ids = !sites.empty()
                       && (*sites.rbegin() != sites.size() - 1 || *sites.begin() != 0);

    if (state->remap_ids) {
        state->internal_to_external_id_mapping.resize(sites.size());

        vert_t internal_id = 0;
        for (auto s : sites) {
            state->internal_to_external_id_mapping[internal_id] = s;
            state->external_to_internal_id_mapping[s] = internal_id;

            internal_id++;
        }
    }

    for (auto mine : mines)
        state->mines.insert(state->toInternalId(mine));

    std::vector<VertexIncidence> incidence_list(sites.size());

    for (auto &incidence : incidence_map) {
        if (!state->remap_ids) {
            incidence_list[incidence.first] = incidence.second;

        } else {
            vert_t from_id = state->toInternalId(incidence.first);
            for (auto &pair : incidence.second) {
                vert_t to_id = state->toInternalId(pair.first);
                incidence_list[from_id][to_id] = pair.second;
            }
        }
    }

    state->incidence_list = incidence_list;
    // TODO: When we read other player's moves, remove edges from here.
    // This is already done in gameState::claimEdge(), so it's wise to use it to record moves.
    state->incidence_available = incidence_list;

    return std::unique_ptr<GameState>(state);
}

void GameState::initPotentials(int depth) {

    std::fill(potential_list.begin(), potential_list.end(), 0);
    potential_list.resize(getSitesNum() * (1 + getMinesNum()), 0);
    for (int i = 0; i < getSitesNum(); i++) {
        potential_t pot = incidence_list[i].size();
        potentialAt(i) = pot; // number of free edges???

        if (site_colors[i])
            potentialAt(i + site_colors[i] * getSitesNum()) = MINE_POTENTIAL;

        //cerr << "pot[" << i << "]=" << pot << endl;
    }

    // propagate
    for (int i = 0; i < depth; i++) {
        std::vector<potential_t> new_potential_list(potential_list);
//        for (int k = 0; k < new_potential_list.size(); k++)
//            new_potential_list[k] *= 1.1;

        for (int i = 0; i < getSitesNum(); i++) {
            for (auto near: incidence_list[i]) {

                if (near.second == -1 || near.second == punter_id) {
                    new_potential_list[i] += potentialAt(near.first) / 10;

                    for (int j = 1; j <= getMinesNum(); j++) {
//                        new_potential_list[i + j*getSitesNum()] += potentialAt(near.first + j*getSitesNum()) / 10;
                        new_potential_list[i + j * getSitesNum()] =
                                max(new_potential_list[i + j * getSitesNum()], potentialAt(near.first + j * getSitesNum()) / 10);
                    }
                }
//                else if(near.second == punter_id)
//                    new_potential_list[i] += potentialAt(near.first) / 10;
            }
        }
        potential_list = new_potential_list;
    }
}

void GameState::colorOurSites() {

    site_colors.resize(getSitesNum(), 0);
    int color = 1;
    for (auto mine: getMines()) {
        site_colors[mine] = color++;
    }

    // color each cluster
    for (auto mine: getMines()) {
        int color = site_colors[mine];
        std::queue<vert_t> wave;
        wave.push(mine);

        while (wave.size()) {
            auto cur = wave.front();
            for (auto edge : getEdgesFrom(cur)) {
                if (edge.second != punter_id || site_colors[edge.first] == color)
                    continue;
                wave.push(edge.first);

                // another region found
                /*
                if(site_colors[edge.first] != 0) {
                    int toRecolor = site_colors[edge.first];
                    for(int i = 0; i < site_colors.size(); i++)
                        if(site_colors[i] == toRecolor)
                            site_colors[i] = color;

                }
                 */
                site_colors[edge.first] = color;
            }
            wave.pop();
        }
    }
}

potential_t GameState::coloredPotentialAt(vert_t i, int curr_color) const {
    potential_t pot = potential_list[i] / 1000;
//    potential_t pot = 0;

    if (site_colors[i] == curr_color)
        return pot;

    int colorsNum = 0;
    potential_t colorPart = 0;
    for (int j = 1; j <= getMinesNum(); j++) {
        colorPart += potential_list[i + j * getSitesNum()];
    }

    return pot + colorPart;
}

std::vector<PotentialEdge> GameState::getMostPotentialEdge() {
// potential algorithm
    colorOurSites();
    initPotentials(10);

    std::unordered_set<vert_t> froms(getOurSites());
    froms.insert(getMines().begin(), getMines().end());

    vector<PotentialEdge> fringeEdges;

    for (auto from : froms) {
        for (auto &edge : getEdgesFrom(from)) {
            auto to = edge.first;

            int from_color = getColors()[from];
            int to_color = getColors()[to];

            if (edge.second == -1 && from_color != getColors()[to])
//                fringeEdges.push_back({coloredPotentialAt(to, to_color), from, to});
                fringeEdges.push_back({coloredPotentialAt(from, to_color)
                                       + coloredPotentialAt(to, from_color), from, to});
        }
    }

    std::sort(fringeEdges.begin(), fringeEdges.end());

    return fringeEdges;
}