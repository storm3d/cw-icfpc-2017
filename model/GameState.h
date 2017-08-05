#ifndef CW_ICFPC_2017_STATE_H
#define CW_ICFPC_2017_STATE_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <cstdint>

typedef uint64_t vert_t;
typedef uint64_t punter_t;

typedef std::unordered_map<vert_t, punter_t> VertexIncidence;

class GameState {
public:
    GameState();

    explicit GameState(std::istream &in);

    void serialize(std::ostream &out) const;

    vert_t getSitesNum() const;

    vert_t getPuntersNum() const;

    vert_t getMinesNum() const;

    punter_t getPunterId() const;

    int getCurrentTurnNum() const;

    bool isEdge(vert_t i, vert_t j) const;

    punter_t getClaimerId(vert_t i, vert_t j) const;

    void claimEdge(vert_t i, vert_t j, punter_t punter);

    bool isMine(vert_t i) const;

    // Whoopsie, I'm exposing implementation.
    const std::unordered_set<vert_t> &getMines() const;

    const std::unordered_map<vert_t, punter_t> &getEdgesFrom(vert_t vertex) const;

    void initMinDistances();

private:

    std::vector<VertexIncidence> incidence_list;

    std::unordered_set<vert_t> mines;

    vert_t punters_num;

    punter_t punter_id;

    int currentTurn;

    // min distance between mines and other vertices
    std::unordered_map<vert_t, std::vector<vert_t>> min_distances;

    friend class GameStateBuilder;
};

class GameStateBuilder {
private:
    std::vector<VertexIncidence> incidence_list;
    std::unordered_set<vert_t> mines;
    vert_t punter_id;
    vert_t punters_num;

public:
    std::vector<VertexIncidence> &incidence_list_ref() {
        return incidence_list;
    }

    std::unordered_set<vert_t> &mines_ref() {
        return mines;
    }

    vert_t& punter_id_ref() {
        return punter_id;
    }

    vert_t& punters_num_ref() {
        return punters_num;
    }

    std::unique_ptr<GameState> build() {
        auto* state = new GameState();

        state->punter_id = punter_id;
        state->punters_num = punters_num;
        state->mines = mines;
        state->incidence_list = incidence_list;

        return std::unique_ptr<GameState>(state);
    }
};

#endif //CW_ICFPC_2017_STATE_H
