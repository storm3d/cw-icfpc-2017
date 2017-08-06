#include "Solver.h"
#include "Empire.h"

StrategyDecision Prolongate::evaluateMove(River r) {
    StrategyDecision decision;
    decision.river = r;
    decision.riskIfNot = 0; // We don't evaluate this.

    const Component *c1 = empire.getByVertex(r.from);
    const Component *c2 = empire.getByVertex(r.from);
    if (c1 != nullptr) {
        Component res = *c1;
        res.addRiver(r);
        score_t oldScore = c1->getScore();

        if (c2 != nullptr) {
            oldScore += c2->getScore();
            res.add(*c2);
        }

        decision.scoreIncrease = res.getScore() - oldScore;
    } else if (c2 != nullptr) {
        Component res = *c2;
        res.addRiver(r);
        decision.scoreIncrease = res.getScore() - c2->getScore();
    } else {
        decision.scoreIncrease = (game.isMine(r.from) || game.isMine(r.to)) ? 1 : 0;
    }

    return decision;
}

StrategyDecision Prolongate::proposedMove() {
    StrategyDecision decision;

    // In this strategy, we're not calculating risks.
    decision.riskIfNot = 0;

    // FIXME: Keep empire.getComponents() sorted by score (or size?).
    for (const Component &compo : empire.getComponents()) {

        for (vert_t v : compo.getVertices()) {

//            if (game.isMine(v)) {
//                // This one is not interested in starting new branches.
//                // Though... TODO: Think of it.
//                continue;
//            }

            for (auto continuation : game.getEdgesFrom(v)) {
                // not claimed.
                if (continuation.second == -1) {
                    vert_t path_next_vertex = continuation.first;
                    Component c2 = compo;

                    c2.addRiver(River(v, path_next_vertex));
                    const Component *c3 = empire.getByVertex(path_next_vertex);
                    if (c3 != nullptr) {
                        if (&compo == c3) {
                            // It would be a loop: the component already contains both vertices.
                            // Just skip it.
                            continue;
                        }
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

StrategyDecision Incept::proposedMove() {
    StrategyDecision decision;
//    double max_mine_risk = 0;

    long unused_mines = std::count_if(
            game.getMines().begin(),
            game.getMines().end(),
            [this](vert_t v) { return this->empire.getByVertex(v) == nullptr; });

    for (vert_t mine : game.getMines()) {

        size_t available_edges = game.getAvailableEdgesFrom(mine).size();
        if (available_edges == 0) {
            // It's already lost to us.
            continue;
        }

        if (empire.getByVertex(mine) != nullptr) {
            // We already have access to this mine.
            // This component could still be blocked from us, but it's a good enough heuristic.
            continue;
        }

        score_t mineRisk = game.getSitesNum() / available_edges;
        if (mineRisk > decision.riskIfNot) {
            decision.riskIfNot = mineRisk;
            decision.scoreIncrease = 1;
            // Just pick the first edge. TODO: Use another strategy to select one, like, whom?..
            auto continuation = game.getAvailableEdgesFrom(mine).begin();
            decision.river = River(mine, continuation->first);
        }
    }

    return decision;
}

StrategyDecision Incept::evaluateMove(River r) {
    StrategyDecision decision;

    return decision;
}

#define RISK_COEFFICIENT 0.5

River Solver::riverToClaim() {

    std::vector<StrategyDecision> decisions(strategies.size());
    std::transform(
            strategies.begin(),
            strategies.end(),
            decisions.begin(),
            [] (ISolverStrategy *s) { return s->proposedMove(); }
    );

    auto decision_it = std::max_element(
            decisions.begin(),
            decisions.end(),
            [](StrategyDecision a, StrategyDecision b) {
                return a.scoreIncrease - a.riskIfNot * RISK_COEFFICIENT
                       > b.scoreIncrease - b.riskIfNot * RISK_COEFFICIENT;
            });

    return decision_it->river;

}

Solver::Solver(GameState &game)
        : game(game),
          empire(game, game.getPunterId()),
          prolongateStrategy(game, empire),
          inceptStrategy(game, empire),
          strategies({&prolongateStrategy, &inceptStrategy})
{
}

void Solver::claimEdge(vert_t i, vert_t j, punter_t punter) {
    empire.claimEdge(i, j, punter);
    for (auto strat : strategies) {
        strat->claimEdge(i, j, punter);
    }
}
