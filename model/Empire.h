#ifndef CW_ICFPC_2017_EMPIRE_H
#define CW_ICFPC_2017_EMPIRE_H

#include "GameState.h"

class Dijkstra;

/// Let's call the entirety of one player's rivers (and mines) an Empire.
/// Empire directly defines the player's score.
class Empire {
	const GameState& game;
	Dijkstra& pathfinder;
	punter_t id;

public:
	Empire(const GameState& game_, Dijkstra& pathfinder_, punter_t punter);

	/// Path in a graph, classic definition. Starts at a mine, ends somewhere.
	typedef std::vector<vert_t> Path;
	// ALL the player's paths.
	const std::vector<Path> getPaths() const;

	long getScore() const;
	long getScore(vert_t from, vert_t to);
};


#endif //CW_ICFPC_2017_EMPIRE_H
