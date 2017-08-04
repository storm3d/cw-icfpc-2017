#include "catch.hpp"

#include <sstream>      // std::istringstream
#include <string>       // std::string

#include "../model/GameState.h"

GameState getSampleGameState()
{
    std::istringstream iss ("{\"punter\":0,\n"
                                    "\"punters\":2,\n"
                                    "\"map\":{\"sites\":[{\"id\":4},{\"id\":1},{\"id\":3},{\"id\":6},{\"id\":5},{\"id\":0},{\"id\":7},{\"id\":2}],\n"
                                    "\"rivers\":[{\"source\":3,\"target\":4},{\"source\":0,\"target\":1},{\"source\":2,\"target\":3},\n"
                                    "{\"source\":1,\"target\":3},{\"source\":5,\"target\":6},{\"source\":4,\"target\":5},\n"
                                    "{\"source\":3,\"target\":5},{\"source\":6,\"target\":7},{\"source\":5,\"target\":7},\n"
                                    "{\"source\":1,\"target\":7},{\"source\":0,\"target\":7},{\"source\":1,\"target\":2}],\n"
                                    "\"mines\":[1,5]}}");
    GameState state(iss);
    return state;
}

TEST_CASE( "GameState.getPlayersNum() should return correct value", "[GameState]" ) {
    REQUIRE( getSampleGameState().getPlayersNum() == 2 );
}

TEST_CASE( "GameState.getSitesNum() should return correct value", "[GameState]" ) {
    REQUIRE( getSampleGameState().getSitesNum() == 8 );
}
