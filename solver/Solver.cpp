#include "Solver.h"
#include "Empire.h"

std::pair<vert_t, vert_t> Solver::riverToClaim(GameState &game) {
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
            if (edge.second == -1) {
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

        for (vert_t v : compo.getVertices()) {

            if (game.isMine(v)) {
                // This one is not interested in starting new branches.
                // Though... TODO: Think of it.
                continue;
            }

            for (auto continuation : game.getEdgesFrom(v)) {
                vert_t path_next_vertex = continuation.first;
                // not claimed.
                if (continuation.second == -1) {

                    Component c2 = compo;
                    c2.addRiver(River(v, path_next_vertex));
                    const Component *c3 = empire.getByVertex(path_next_vertex);
                    if (c3 != nullptr) {
                        assert(&compo != c3); // otherwise it's already claimed by us.
                        c2.add(*c3);
                    }

                    score_t scoreIncrease = c2.getScore() - compo.getScore();
                    if (scoreIncrease > decision.scoreIncrease) {
                        decision.river = {v, path_next_vertex};
                        decision.scoreIncrease = scoreIncrease;
                    }
                }
            }
        }

    }

    return decision;
}
