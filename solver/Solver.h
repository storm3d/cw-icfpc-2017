#ifndef CW_ICFPC_2017_SOLVER_H
#define CW_ICFPC_2017_SOLVER_H

#include "GameState.h"
#include "Empire.h"


/** A decision proposed by a strategy */
struct StrategyDecision {
    River river;

    /** Immediate score increase */
    score_t scoreIncrease{0};

    /**
     * How much score we're risking if we don't take this move.
     * Like, how much we'll lose on an unclaimed Future.
     * Multiplied by probability... or should we account for probability separately?
     * */
    score_t riskIfNot{0};

    inline bool isEmpty() const { return river == River::EMPTY; };

    StrategyDecision() : river(River::EMPTY) {}
};

class ISolverStrategy : public IGameUpdater {
protected:
    GameState &game;
    Empire &empire;

public:
    ISolverStrategy(GameState &game, Empire &empire) : game(game), empire(empire) {};

    /** Select a move somehow */
    virtual StrategyDecision proposedMove() = 0;
    /** Evaluate a move, maybe proposed by different strategy */
    virtual StrategyDecision evaluateMove(River r) = 0;

    virtual ~ISolverStrategy() = default;

    void claimEdge(vert_t i, vert_t j, punter_t punter) override {}
};

struct Prolongate : public ISolverStrategy {
    Prolongate(GameState &game, Empire &empire) : ISolverStrategy(game, empire) {};
    StrategyDecision proposedMove() override;
    StrategyDecision evaluateMove(River r) override;
};

struct Incept : public ISolverStrategy {
    Incept(GameState &game, Empire &empire) : ISolverStrategy(game, empire) {};
    StrategyDecision proposedMove() override;
    /** @deprecated Not implemented yet */
    StrategyDecision evaluateMove(River r) override;
};

//struct CompositeSolver : public ISolverStrategy {
//    CompositeSolver(GameState &game, Empire &empire) : ISolverStrategy(game) {};
//    StrategyDecision proposedMove() override;
//    /** @deprecated Not implemented yet */
//    StrategyDecision evaluateMove(River r) override;
//};

class Solver : public IGameUpdater {
    GameState &game;
    Empire empire;
    Prolongate prolongateStrategy;
    Incept inceptStrategy;
    std::vector<ISolverStrategy *> strategies;

public:
    explicit Solver(GameState &game);
	River riverToClaim();

    void claimEdge(vert_t i, vert_t j, punter_t punter) override;
};

#endif //CW_ICFPC_2017_SOLVER_H
