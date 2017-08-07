#ifndef CW_ICFPC_2017_STATE_H
#define CW_ICFPC_2017_STATE_H

#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <cstdint>
#include <memory>

#include <json.hpp>

typedef int vert_t;
typedef int punter_t;
typedef float potential_t;

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

struct PotentialEdge {
    potential_t  pot;
    vert_t from;
    vert_t to;
};

inline bool operator< (const PotentialEdge& lhs, const PotentialEdge& rhs){ return lhs.pot>rhs.pot; }

/**
 * A bit ugly way to keep all the
 */
struct IGameUpdater {
    virtual void claimEdge(vert_t i, vert_t j, punter_t punter) = 0;
};

// Work around LLDB aggressive
template
class std::vector<vert_t>;

template
class std::unordered_map<vert_t, std::vector<vert_t>>;


class GameState : public IGameUpdater {
public:
    static clock_t punter_begin_time;
    const float PANIC_TIME = 0.85;

    GameState();

    explicit GameState(std::istream &in);

    explicit GameState(std::string json);

    void serialize(std::ostream &out) const;

    void deserialize(std::istream &in);

    void deserialize(nlohmann::json &state);

    vert_t getSitesNum() const;

    vert_t getPuntersNum() const;

    vert_t getMinesNum() const;

    punter_t getPunterId() const;

    int getCurrentTurnNum() const;

    bool isEdge(vert_t i, vert_t j) const;

    punter_t getClaimerId(vert_t i, vert_t j) const;

    void claimEdge(vert_t i, vert_t j, punter_t punter);

    bool isMine(vert_t i) const;

    potential_t &potentialAt(vert_t i) {return potential_list[i];}
    potential_t coloredPotentialAt(vert_t i, int curr_color) const;
    std::vector<PotentialEdge> getMostPotentialEdge();

    void initPotentials(int depth);
    void colorOurSites();
    const std::vector<potential_t> &getPotentials() const {return potential_list;}
    const std::unordered_set<vert_t> &getOurSites() const {return our_sites;}
    const std::vector<int> &getColors() const {return site_colors;}

    // Whoopsie, I'm exposing implementation.
    const std::unordered_set<vert_t> &getMines() const;

    const std::unordered_map<vert_t, punter_t> &getEdgesFrom(vert_t vertex) const;

    /// FIXME: Repeated group of field/method. Separate into a class (and extract Map class too).
    const std::unordered_map<vert_t, punter_t> &getAvailableEdgesFrom(vert_t vertex) const;

    const std::unordered_map<vert_t, std::vector<vert_t>> &getMinDistances() const;

    //void complementEdges();

    void initMinDistances();

    const vert_t toInternalId(vert_t externalId);

    const vert_t toExternalId(vert_t internalId);

private:
    bool remap_ids;
    std::vector<vert_t> internal_to_external_id_mapping;
    std::unordered_map<vert_t, vert_t> external_to_internal_id_mapping;

    std::vector<VertexIncidence> incidence_list;

    /** Map that is still available to us (claimed by us or unclaimed) */
    std::vector<VertexIncidence> incidence_available;

    std::unordered_set<vert_t> mines;

    std::vector<potential_t> potential_list;
    std::vector<int> site_colors;
    std::unordered_set<vert_t> our_sites;

    vert_t punters_num = 0;

    punter_t punter_id = 0;

    int currentTurn;

    // min distance between mines and other vertices
    std::unordered_map<vert_t, std::vector<vert_t>> min_distances;

    friend class GameStateBuilder;
};

std::istream &operator<<(std::istream &in, GameState &game);

class GameStateBuilder {
private:
    std::set<vert_t> sites;
    std::map<vert_t, VertexIncidence> incidence_map;
    std::unordered_set<vert_t> mines;
    vert_t punter_id = 0;
    vert_t punters_num = 0;

public:
    void add_river(vert_t from, vert_t to, punter_t punter);

    std::set<vert_t> &sites_ref() {
        return sites;
    }

    std::unordered_set<vert_t> &mines_ref() {
        return mines;
    }

    vert_t &punter_id_ref() {
        return punter_id;
    }

    vert_t &punters_num_ref() {
        return punters_num;
    }

    std::unique_ptr<GameState> build();
};

#endif //CW_ICFPC_2017_STATE_H
