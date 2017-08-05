#include "Solver.h"
#include "Empire.h"

class Dijkstra {
};

std::pair<vert_t, vert_t> Solver::riverToClaim(GameState &game) {
    Dijkstra stub;
    Empire empire(game, 0);

    // 1. Continue whatever longest path I have.
    std::pair<vert_t, vert_t> candidate = {0, 0};
    long bestScoreIncrease = 0;

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

StrategyDecision Prolongate::proposedMove(GameState &game) {
    // TODO: Move to fields.
    Empire empire(game, game.getPunterId());
    StrategyDecision decision;

    // In this strategy, we're not calculating risks.
    decision.riskIfNot = 0;

    // FIXME: Keep empire.getComponents() sorted by score (or size?).
    for (const Component &compo : empire.getComponents()) {

        for (const River &r : compo.getRivers()) {
            vert_t river_from = r.from;
            vert_t river_end = r.to;

            for (auto continuation : game.getEdgesFrom(river_end)) {
                vert_t path_next_vertex = continuation.first;
                // not claimed.
                if (continuation.second == 0) {

                    auto newEmpire = empire;
                    newEmpire.addRiver(r);

                    score_t scoreIncrease = newEmpire.getScore() - empire.getScore();
                    if (scoreIncrease > decision.scoreIncrease) {
                        decision.river = {river_end, path_next_vertex};
                        decision.scoreIncrease = scoreIncrease;
                    }
                }
            }
        }

    }

    return decision;
}
