#include <catch.hpp>
#include <json.hpp>

#include <sstream>
#include <string>

#include "../model/GameState.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ClangTidyInspection"

using nlohmann::json;

static GameState deserializeState()
{
    std::istringstream iss ("{\"punter\":0,\n"
                                    "\"punters\":2,\n"
                                    "\"map\":{\"sites\":[{\"id\":4},{\"id\":1},{\"id\":3},{\"id\":6},{\"id\":5},{\"id\":0},{\"id\":7},{\"id\":2}],\n"
                                    "\"rivers\":[{\"source\":3,\"target\":4,\"punter\":0},{\"source\":0,\"target\":1,\"punter\":0},{\"source\":2,\"target\":3,\"punter\":0},\n"
                                    "{\"source\":1,\"target\":3,\"punter\":0},{\"source\":5,\"target\":6,\"punter\":0},{\"source\":4,\"target\":5,\"punter\":0},\n"
                                    "{\"source\":3,\"target\":5,\"punter\":0},{\"source\":6,\"target\":7,\"punter\":0},{\"source\":5,\"target\":7,\"punter\":0},\n"
                                    "{\"source\":1,\"target\":7,\"punter\":0},{\"source\":0,\"target\":7,\"punter\":0},{\"source\":1,\"target\":2,\"punter\":0}],\n"
                                    "\"mines\":[1,5]}}");

    json j;
    iss >> j;

    GameState state;
    state.deserialize(j);
    return state;
}

TEST_CASE( "GameState.getPuntersNum() should return correct value", "[GameState]" ) {
    REQUIRE(deserializeState().getPuntersNum() == 2 );
}

TEST_CASE( "GameState.getSitesNum() should return correct value", "[GameState]" ) {
    REQUIRE(deserializeState().getSitesNum() == 8 );
}

TEST_CASE( "GameState.getPunterId() should return correct value", "[GameState]" ) {
    REQUIRE(deserializeState().getPunterId() == 0 );
}

TEST_CASE( "GameState.isEdge() should return correct value", "[GameState]" ) {
    REQUIRE(deserializeState().isEdge(3, 4) );
    REQUIRE( !deserializeState().isEdge(3, 7) );
}

TEST_CASE( "GameState.isMine() should return correct value", "[GameState]" ) {
    REQUIRE(deserializeState().isMine(5) );
    REQUIRE( !deserializeState().isMine(2) );
}

TEST_CASE( "GameState serialization" ) {
    std::ostringstream os;
    deserializeState().serialize(os);

    std::string s = os.str();

    nlohmann::json jee = nlohmann::json::parse(s);
    REQUIRE( jee["punters"] == 2 );
    REQUIRE( jee["punter"] == 0 );
    REQUIRE( jee["map"]["mines"].size() == 2 );
}

TEST_CASE("GameState serialization contains river punters") {
    const vert_t PUNTER_1 = 101;
    const vert_t PUNTER_2 = 201;

    GameStateBuilder builder;
    builder.incidence_list_ref().resize(3);
    builder.incidence_list_ref()[0][1] = PUNTER_1;
    builder.incidence_list_ref()[0][2] = PUNTER_2;

    std::stringstream ss;
    builder.build()->serialize(ss);

    std::cout << ss.str();
    ss.seekg(0, std::ios::beg);

    GameState state;
    state.deserialize(ss);

    REQUIRE(state.getEdgesFrom(0).size() == 2);
    REQUIRE(state.getEdgesFrom(0).at(1) == PUNTER_1);
    REQUIRE(state.getEdgesFrom(0).at(2) == PUNTER_2);
}

TEST_CASE("Game state has complemented adjucency list") {
    // site -- site -- site
    GameStateBuilder builder;
    builder.incidence_list_ref().resize(3);
    builder.incidence_list_ref()[0][1] = 0;
    builder.incidence_list_ref()[1][2] = 0;

    auto state = builder.build();
    REQUIRE(state->getEdgesFrom(0).size() == 1);
    REQUIRE(state->getEdgesFrom(0).at(1) == 0);

    REQUIRE(state->getEdgesFrom(1).size() == 2);
    REQUIRE(state->getEdgesFrom(1).at(0) == 0);
    REQUIRE(state->getEdgesFrom(1).at(2) == 0);

    REQUIRE(state->getEdgesFrom(2).size() == 1);
    REQUIRE(state->getEdgesFrom(2).at(1) == 0);

}

// TODO Fix getMinDistances to make this test pass
//TEST_CASE("Calculate min distances to mines") {
//    // mine -- site -- site
//    GameStateBuilder builder;
//    builder.mines_ref().insert(0);
//    builder.incidence_list_ref().resize(3);
//    builder.incidence_list_ref()[0][1] = 0;
//    builder.incidence_list_ref()[1][0] = 0;
//    builder.incidence_list_ref()[1][2] = 0;
//    builder.incidence_list_ref()[2][1] = 0;
//
//    auto state = builder.build();
//
//    state->initMinDistances();
//
//    REQUIRE(state->getMinDistances().size() == 1);
//    REQUIRE(state->getMinDistances().at(0)[0] == 0);
//    REQUIRE(state->getMinDistances().at(0)[1] == 1);
//    REQUIRE(state->getMinDistances().at(0)[2] == 2);
//}

#pragma clang diagnostic pop
