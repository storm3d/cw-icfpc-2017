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
    GameState(std::istream &in);
	void Serialize(std::ostream &in);

    vert_t getSitesNum();
    vert_t getPlayersNum();
    vert_t getMinesNum();
    int getCurrentTurnNum();

    bool isEdge(vert_t i, vert_t j);
	punter_t getClaimerId(vert_t i, vert_t j);

    void claimEdge(vert_t i, vert_t j, punter_t punter);

    bool isMine(vert_t i);

	// Whoopsie, I'm exposing implementation.
	const std::unordered_set<vert_t>& getMines() const;

	const std::unordered_map<vert_t, punter_t>& getEdgesFrom(vert_t vertex) const;

private:

	std::vector<VertexIncidence> incidence_list;

	std::unordered_set<vert_t> mines;

    vert_t playersNum;

    int punterId;

    int currentTurn;

	// min distance between vertices
	std::vector<std::vector<vert_t>> min_distance;
};

#endif //CW_ICFPC_2017_STATE_H
