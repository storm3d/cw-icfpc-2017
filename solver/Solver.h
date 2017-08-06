#ifndef CW_ICFPC_2017_SOLVER_H
#define CW_ICFPC_2017_SOLVER_H

#include "GameState.h"
#include "Empire.h"


/** A decision proposed by a strategy */
struct StrategyDecision {
    River river;
    /** Immediate score increase */
    score_t scoreIncrease;
    /**
     * How much score we're risking if we don't take this move.
     * Like, how much we'll lose on an unclaimed Future.
     * */
    long riskIfNot;

    inline bool isEmpty() const { return river == River::EMPTY; };

    StrategyDecision() : river(River::EMPTY), scoreIncrease(0), riskIfNot(0) {}
};

struct ISolverStrategy {
    virtual StrategyDecision proposedMove(GameState &game) = 0;

    virtual ~ISolverStrategy() = default;
};

struct Prolongate : public ISolverStrategy {
    StrategyDecision proposedMove(GameState &game) override;

    virtual ~Prolongate() = default;
};

class Solver {
public:
	std::pair<vert_t, vert_t> riverToClaim(GameState &game);
};

#endif //CW_ICFPC_2017_SOLVER_H
