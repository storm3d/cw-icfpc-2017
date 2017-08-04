#include "Solver.h"
#include "Empire.h"

class Dijkstra {
};

Solver::Solver(punter_t id) : id(id) {}

std::pair<vert_t, vert_t> Solver::riverToClaim(GameState &game) {
	Dijkstra stub;

	Empire empire(game, stub, id);

	// 1. Continue whatever longest path I have.
	std::pair<vert_t, vert_t> candidate = {0, 0};
	long bestScoreIncrease = 0;

	for (auto path : empire.getPaths()) {
		vert_t path_mine = path.front();
		vert_t path_end = path.back();
		for (auto continuation : game.getEdgesFrom(path_end)) {
			vert_t path_next_vertex = continuation.first;
			// not claimed.
			if (continuation.second == 0) {
				long scoreIncrease = empire.getScore(path_mine, path_next_vertex) - empire.getScore(path_mine, path_end);
				if (scoreIncrease > bestScoreIncrease) {
					bestScoreIncrease = scoreIncrease;
					candidate = {path_end, path_next_vertex};
				}
			}
		}
	}

	if (bestScoreIncrease > 0) {
		return candidate;
	}

	// 2. Claim whatever free river adjacent to a mine.
	for (auto mine : game.getMines()) {
		for (auto edge : game.getEdgesFrom(mine)) {
			if (edge.second == 0) {
				return {mine, edge.first};
			}
		}
	}

	// 3. If none exist, just pass - we cannot gain any more score.


	return candidate;
}
