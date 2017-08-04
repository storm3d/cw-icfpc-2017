#ifndef CW_ICFPC_2017_SOLVER_H
#define CW_ICFPC_2017_SOLVER_H

#include "GameState.h"

class Solver {
	punter_t id;
public:
	Solver(punter_t id);

	std::pair<vert_t, vert_t> riverToClaim(GameState &game);
};


#endif //CW_ICFPC_2017_SOLVER_H
