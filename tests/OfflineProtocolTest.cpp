
#include <catch.hpp>
#include <json.hpp>

#include <sstream>
#include <string>

#include "../model/GameState.h"
#include "../punter/OfflineProtocol.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ClangTidyInspection"

using nlohmann::json;

static GameState getSampleGameStateFromSetupRequest()
{
    std::istringstream iss ("{\"punter\":0,\n"
                                    "\"punters\":2,\n"
                                    "\"map\":{\"sites\":[{\"id\":4},{\"id\":1},{\"id\":3},{\"id\":6},{\"id\":5},{\"id\":0},{\"id\":7},{\"id\":2}],\n"
                                    "\"rivers\":[{\"source\":3,\"target\":4},{\"source\":0,\"target\":1},{\"source\":2,\"target\":3},\n"
                                    "{\"source\":1,\"target\":3},{\"source\":5,\"target\":6},{\"source\":4,\"target\":5},\n"
                                    "{\"source\":3,\"target\":5},{\"source\":6,\"target\":7},{\"source\":5,\"target\":7},\n"
                                    "{\"source\":1,\"target\":7},{\"source\":0,\"target\":7},{\"source\":1,\"target\":2}],\n"
                                    "\"mines\":[1,5]}}");

    json request;
    iss >> request;

    OfflineProtocol protocol;

    return *protocol.getStateFromSetupRequest(request).get();
}

TEST_CASE( "Setup Request GameState.getPuntersNum() should return correct value", "[GameState]" ) {
    REQUIRE(getSampleGameStateFromSetupRequest().getPuntersNum() == 2 );
}

TEST_CASE( "Setup Request GameState.getSitesNum() should return correct value", "[GameState]" ) {
    REQUIRE(getSampleGameStateFromSetupRequest().getSitesNum() == 8 );
}

TEST_CASE( "Setup Request GameState.getPunterId() should return correct value", "[GameState]" ) {
    REQUIRE(getSampleGameStateFromSetupRequest().getPunterId() == 0 );
}

TEST_CASE( "Setup Request GameState.isEdge() should return correct value", "[GameState]" ) {
    REQUIRE(getSampleGameStateFromSetupRequest().isEdge(3, 4) );
    REQUIRE( !getSampleGameStateFromSetupRequest().isEdge(3, 7) );
}

TEST_CASE( "Setup Request GameState.isMine() should return correct value", "[GameState]" ) {
    REQUIRE(getSampleGameStateFromSetupRequest().isMine(5) );
    REQUIRE( !getSampleGameStateFromSetupRequest().isMine(2) );
}

TEST_CASE( "Setup Request GameState serialization" ) {
    std::ostringstream os;
    getSampleGameStateFromSetupRequest().serialize(os);

    std::string s = os.str();

    nlohmann::json jee = nlohmann::json::parse(s);
    REQUIRE( jee["punters"] == 2 );
    REQUIRE( jee["punter"] == 0 );
    REQUIRE( jee["map"]["mines"].size() == 2 );
}

#pragma clang diagnostic pop

