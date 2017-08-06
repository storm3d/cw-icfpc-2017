#include <catch.hpp>
#include "../model/GameState.h"
#include "../solver/Solver.h"
#include "../punter/OfflineProtocol.h"

using std::string;

namespace Catch {
    template<> std::string toString<River>(River const& r) {
        return string("{") + std::to_string(r.from) + "->" + std::to_string(r.to) + "}";
    }
}

string problem = R"(
{
  "punter":0,
  "punters":2,
  "map":{
    "sites":[
       {"id":0,"x":0,"y":0},{"id":1,"x":1,"y":0},{"id":2,"x":2,"y":0},{"id":3,"x":2,"y":-1},{"id":4,"x":2,"y":-2},{"id":5,"x":1,"y":-2},{"id":6,"x":0,"y":-2},{"id":7,"x":0,"y":-1}
    ],
    "rivers":[
       {"source":0,"target":1},{"source":1,"target":2},{"source":0,"target":7},{"source":7,"target":6},{"source":6,"target":5},{"source":5,"target":4},{"source":4,"target":3},{"source":3,"target":2},{"source":1,"target":7},{"source":1,"target":3},{"source":7,"target":5},{"source":5,"target":3}
    ],
    "mines":[1,5]
  }
})";

TEST_CASE("'Prolongate existing' strategy") {

    OfflineProtocol dummy;
    json j = json::parse(problem);
    auto game = dummy.extractStateFromSetupRequest(j);
    game->initMinDistances();
    Prolongate prolonger;

    SECTION("This one is not supposed to work on completely empty graph") {
        StrategyDecision d = prolonger.proposedMove(*game.get());
    }

    game->claimEdge(0, 1, game->getPunterId());

    StrategyDecision d = prolonger.proposedMove(*game.get());
    CAPTURE(d.river);
    REQUIRE(d.scoreIncrease == 1);
    REQUIRE(d.river == River(0, 7)); // bad, bad assertion! We dunno what river it will pick.

    game->claimEdge(d.river.from, d.river.to, game->getPunterId());

    d = prolonger.proposedMove(*game.get());
    CAPTURE(d.river);
    REQUIRE(d.river == River(2, 3));
    REQUIRE(d.scoreIncrease == 4);

    game->claimEdge(d.river.from, d.river.to, game->getPunterId());

    d = prolonger.proposedMove(*game.get());
    CAPTURE(d.river);
//    REQUIRE(d.river == River(2, 3));
    REQUIRE(d.scoreIncrease == 9);
}
