#ifndef CW_ICFPC_2017_EMPIRE_H
#define CW_ICFPC_2017_EMPIRE_H

#include <cstdint>
#include "GameState.h"

typedef uint64_t score_t;

/**
 * A connectivity component of the map.
 * */
class Component {
private:
    score_t score;
    const GameState *game;
    std::unordered_set<vert_t> vertices;
    std::vector<River> rivers;

    void recalculateScore();
public:
    explicit Component(const GameState *game, vert_t punterId);
    Component(const Component &other) = default;
//    Component & operator= (const Component &);

    void addRiver(const River &r);
    void add(const Component &other);

    const std::vector<River> & getRivers() const;

    bool isAdjacent(const River &r) const;
    long distanceTo(vert_t vertex) const;
    long distanceTo(const Component &other) const;

    score_t getScore() const;
};


/// Let's call the entirety of one player's rivers (and mines) an Empire.
/// Empire directly defines the player's score.
class Empire {
    std::vector<Component> components;
    const GameState& game;
    vert_t punterId;

public:
    explicit Empire(const GameState &game_, vert_t punterId);

    void addRiver(const River &river);

    // ALL the player's paths.
    const std::vector<Component> & getComponents() const;

    score_t getScore() const;
};


#endif //CW_ICFPC_2017_EMPIRE_H
