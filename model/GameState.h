#ifndef CW_ICFPC_2017_STATE_H
#define CW_ICFPC_2017_STATE_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <cstdint>
#include <memory>

#include <json.hpp>

typedef uint64_t vert_t;
typedef int64_t punter_t;

typedef std::unordered_map<vert_t, punter_t> VertexIncidence;

struct River {
    vert_t from;
    vert_t to;
    River(vert_t from, vert_t to);

    bool operator==(const River &rhs) const;

    bool operator!=(const River &rhs) const;

    bool isAdjacent(const River &other) const;
    bool contains(vert_t v) const;

    static const River EMPTY;
};

/**
 * A bit ugly way to keep all the
 */
struct IGameUpdater {
    virtual void claimEdge(vert_t i, vert_t j, punter_t punter) = 0;
};

// Work around LLDB aggressive
template class std::vector<vert_t>;
template class std::unordered_map<vert_t, std::vector<vert_t>>;


class GameState : public IGameUpdater {
public:
    GameState();
    explicit GameState(std::istream &in);
    explicit GameState(std::string json);

    void serialize(std::ostream &out) const;

    void deserialize(std::istream &in);

    void deserialize(nlohmann::json& state);

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

    /// FIXME: Repeated group of field/method. Separate into a class (and extract Map class too).
    const std::unordered_map<vert_t, punter_t> &getAvailableEdgesFrom(vert_t vertex) const;

    const std::unordered_map<vert_t, std::vector<vert_t>> &getMinDistances() const;

    //void complementEdges();

    void initMinDistances();

private:

    std::vector<VertexIncidence> incidence_list;

    /** Map that is still available to us (claimed by us or unclaimed) */
    std::vector<VertexIncidence> incidence_available;

    std::unordered_set<vert_t> mines;

    vert_t punters_num = 0;

    punter_t punter_id = 0;

    int currentTurn;

    // min distance between mines and other vertices
    std::unordered_map<vert_t, std::vector<vert_t>> min_distances;

    friend class GameStateBuilder;
};

std::istream & operator << (std::istream &in, GameState& game);

class GameStateBuilder {
private:
    std::vector<VertexIncidence> incidence_list;
    std::unordered_set<vert_t> mines;
    vert_t punter_id = 0;
    vert_t punters_num = 0;

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
        // TODO: When we read other player's moves, remove edges from here.
        // This is already done in gameState::claimEdge(), so it's wise to use it to record moves.
        state->incidence_available = incidence_list;

        // NO NEED TO DO THIS, we do it on deserialization
        //state->complementEdges();

        return std::unique_ptr<GameState>(state);
    }
};

#endif //CW_ICFPC_2017_STATE_H
