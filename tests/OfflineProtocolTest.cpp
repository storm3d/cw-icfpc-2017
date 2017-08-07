
#include <catch.hpp>
#include <json.hpp>

#include <sstream>
#include <string>

#include "../model/GameState.h"
#include "../punter/OfflineProtocol.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ClangTidyInspection"

using nlohmann::json;

static GameState sampleStateFromSetupRequest() {
    std::istringstream iss("{\"punter\":0,\n"
                                   "\"punters\":2,\n"
                                   "\"map\":{\"sites\":[{\"id\":4},{\"id\":1},{\"id\":3},{\"id\":6},{\"id\":5},{\"id\":0},{\"id\":7},{\"id\":2}],\n"
                                   "\"rivers\":[{\"source\":3,\"target\":4},{\"source\":0,\"target\":1},{\"source\":2,\"target\":3},\n"
                                   "{\"source\":1,\"target\":3},{\"source\":5,\"target\":6},{\"source\":4,\"target\":5},\n"
                                   "{\"source\":3,\"target\":5},{\"source\":6,\"target\":7},{\"source\":5,\"target\":7},\n"
                                   "{\"source\":1,\"target\":7},{\"source\":0,\"target\":7},{\"source\":1,\"target\":2}],\n"
                                   "\"mines\":[1,5]}}");

    OfflineProtocol protocol;
    return *protocol.extractStateFromSetupRequest(iss).get();
}

static GameState sparseStateFromSetupRequest() {
    std::istringstream iss("{\"punter\":0,\n"
                                   "\"punters\":2,\n"
                                   "\"map\":{\"sites\":[{\"id\":5},{\"id\":10}],\n"
                                   "\"rivers\":[{\"source\":5,\"target\":10}],\n"
                                   "\"mines\":[10]}}");

    OfflineProtocol protocol;
    return *protocol.extractStateFromSetupRequest(iss).get();
}

static std::vector<OfflineProtocol::Move> extractSampleMovesFromMoveRequest() {

    std::istringstream iss(R"({"move":{"moves":[{"claim":{"punter":5,"source":0,"target":1}},
{"claim":{"punter":6,"source":1,"target":2}},
{"pass":{"punter":7}}]}})");

    json request;
    iss >> request;

    OfflineProtocol protocol;

    return protocol.extractMovesFromMoveRequest(request);
}

static std::vector<OfflineProtocol::Move> extractSampleMovesFromMoveRequestSplurges() {

    std::istringstream iss(R"({"move":{"moves":[
{"splurge":{"punter":3,"route":[9,10,11,12,14]}},
{"claim":{"punter":5,"source":0,"target":1}},
{"claim":{"punter":6,"source":1,"target":2}},
{"pass":{"punter":7}}
]}})");

    json request;
    iss >> request;

    OfflineProtocol protocol;

    return protocol.extractMovesFromMoveRequest(request);
}


TEST_CASE("Setup Request GameState.getPuntersNum() should return correct value", "[GameState]") {
    REQUIRE(sampleStateFromSetupRequest().getPuntersNum() == 2);
}

TEST_CASE("Setup Request GameState.getSitesNum() should return correct value", "[GameState]") {
    REQUIRE(sampleStateFromSetupRequest().getSitesNum() == 8);
}

TEST_CASE("Setup Request GameState.getPunterId() should return correct value", "[GameState]") {
    REQUIRE(sampleStateFromSetupRequest().getPunterId() == 0);
}

TEST_CASE("Setup Request GameState.isEdge() should return correct value", "[GameState]") {
    REQUIRE(sampleStateFromSetupRequest().isEdge(3, 4));
    REQUIRE(!sampleStateFromSetupRequest().isEdge(3, 7));
}

TEST_CASE("Setup Request GameState.isMine() should return correct value", "[GameState]") {
    REQUIRE(sampleStateFromSetupRequest().isMine(5));
    REQUIRE(!sampleStateFromSetupRequest().isMine(2));
}

TEST_CASE("Setup Request GameState serialization") {
    std::ostringstream os;
    sampleStateFromSetupRequest().serialize(os);

    std::string s = os.str();

    nlohmann::json jee = nlohmann::json::parse(s);
    REQUIRE(jee["punters"] == 2);
    REQUIRE(jee["punter"] == 0);
    REQUIRE(jee["map"]["mines"].size() == 2);
}

TEST_CASE("Move Request should extract only claimed edges") {
    REQUIRE(extractSampleMovesFromMoveRequest().size() == 2);
    REQUIRE(extractSampleMovesFromMoveRequest()[0] == OfflineProtocol::Move(5, 0, 1));
    REQUIRE(extractSampleMovesFromMoveRequest()[1] == OfflineProtocol::Move(6, 1, 2));

    REQUIRE(extractSampleMovesFromMoveRequestSplurges().size() == 6);
    REQUIRE(extractSampleMovesFromMoveRequestSplurges()[0] == OfflineProtocol::Move(3, 9, 10));
    REQUIRE(extractSampleMovesFromMoveRequestSplurges()[1] == OfflineProtocol::Move(3, 10, 11));
    REQUIRE(extractSampleMovesFromMoveRequestSplurges()[2] == OfflineProtocol::Move(3, 11, 12));
    REQUIRE(extractSampleMovesFromMoveRequestSplurges()[3] == OfflineProtocol::Move(3, 12, 14));
    REQUIRE(extractSampleMovesFromMoveRequestSplurges()[4] == OfflineProtocol::Move(5, 0, 1));
    REQUIRE(extractSampleMovesFromMoveRequestSplurges()[5] == OfflineProtocol::Move(6, 1, 2));
}

TEST_CASE("Setup Request sparse site ids") {

    GameState state = sparseStateFromSetupRequest();

    REQUIRE(state.toInternalId(5) == 0);
    REQUIRE(state.toInternalId(10) == 1);

    REQUIRE(state.getSitesNum() == 2);

    REQUIRE(state.getMines().size() == 1);
    REQUIRE(state.getMines().find(1) != state.getMines().end());

    REQUIRE(state.isEdge(0, 1));
    REQUIRE(state.isEdge(1, 0));
}

#pragma clang diagnostic pop
