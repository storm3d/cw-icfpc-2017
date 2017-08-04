#include "catch.hpp"

#include <sstream>      // std::istringstream
#include <string>       // std::string

#include "../model/GameState.h"

vert_t getGameStatePlayersNum()
{
    std::istringstream iss ("{\"punters\" : 2}");
    GameState state(iss);
    return state.getPlayersNum();
}

TEST_CASE( "getPlayersNum() should return correct value", "[GameState]" ) {
    REQUIRE( getGameStatePlayersNum() == 2 );
}
