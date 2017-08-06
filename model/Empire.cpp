#include <cassert>
#include "Empire.h"

Empire::Empire(const GameState &game_, vert_t punterId)
        : game(game_), punterId(punterId)
{
    for (vert_t v = 0; v < game.getSitesNum(); v++) {
        for (auto river_from_v : game.getEdgesFrom(v)) {
            if (river_from_v.second == punterId) {
                addRiver(River(v, river_from_v.first));
            }
        }
    }
}

void Empire::addRiver(const River &river) {
    auto first_adjacent = components.end();

    for (auto it = components.begin(); it != components.end(); it++) {
        if (it->isAdjacent(river)) {
            if (first_adjacent != components.end()) {
                // Merge two components joined by the river.
                first_adjacent->add(*it);
                components.erase(it);
                return;
            }

            it->addRiver(river);
            first_adjacent = it;
        }
    }

    if (first_adjacent == components.end()) {
        components.push_back(Component(&game, punterId));
    }
}

const std::vector<Component> & Empire::getComponents() const {
    return components;
}

score_t Empire::getScore() const {
    score_t result = 0;
    for (const auto &c : components) {
        result += c.getScore();
    }
    return result;
}

Component::Component(const GameState *game, vert_t punterId) : game(game), score(0) {
    for (vert_t v = 0; v < game->getSitesNum(); v++) {
        for (auto river_from_v : game->getEdgesFrom(v)) {
            if (river_from_v.second == punterId) {
                addRiver(River(v, river_from_v.first));
            }
        }
    }

    recalculateScore();
}

void Component::addRiver(const River &r) {
    auto status_from = vertices.insert(r.from);
    auto status_to = vertices.insert(r.to);

    // The river is not repeated
    if (status_from.second || status_to.second) {
        rivers.push_back(r);
    }

    recalculateScore();
}

void Component::add(const Component &other) {
    for (auto r : other.rivers) {
        addRiver(r);
    }

    recalculateScore();
}

bool Component::isAdjacent(const River &r) const {
    return vertices.find(r.from) != vertices.end()
           || vertices.find(r.to) != vertices.end();
}

score_t Component::getScore() const {
    return score;
}

void Component::recalculateScore() {
    score = 0;
    for (auto mine : vertices) {
        if (!game->isMine(mine)) continue;
        
        for (auto v : vertices) {
            auto d = game->getMinDistances().at(mine)[v];
            score += d*d;
        }
    }
}

long Component::distanceTo(vert_t vertex) const {
    long result = game->getSitesNum();
    for (auto v : vertices) {
        auto d = game->getMinDistances().at(vertex)[v];
        if (d < result) {
            result = d;
        }
    }

//    So much about C++11.
//    auto it = std::min_element(
//            vertices.begin(), vertices.end(),
//            [vertex] (vert_t a) {
//                return game.getMinDistances().at(vertex)[a] < game.getMinDistances().at(vertex)[b];
//            });

    return result;
}

// Could be optimized a lot, as the distance between connectivity components can be saved in distances matrix,
// or computed with Dijkstra on a different graph.
// See also: https://en.wikipedia.org/wiki/Dynamic_connectivity
long Component::distanceTo(const Component &other) const {
    long result = game->getSitesNum();

    for (auto v : vertices) {
        auto d = other.distanceTo(v);
        if (d < result) {
            result = d;
        }
    }

    return result;
}

const std::vector<River> &Component::getRivers() const {
    return rivers;
}
