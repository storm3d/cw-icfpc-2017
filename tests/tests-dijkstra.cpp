#include <vector>
#include <array>

using namespace std;

#include "../include/catch.hpp"
#include "../model/Dijkstra.h"


TEST_CASE("no adjacent vertices") {
    unsigned int vertices = 3;

    vector<vector<vert_t>> adj(vertices);

    vector<vert_t> min_distance(vertices);

    Dijkstra(1, vertices, adj, min_distance);

    REQUIRE(min_distance[1] == 0);
    REQUIRE(min_distance[0] == INT_MAX);
    REQUIRE(min_distance[2] == INT_MAX);
}


TEST_CASE("two adjacent vertices 0 - 1 - 2") {
    unsigned int vertices = 3;

    vector<vector<vert_t>> adj(vertices);
    adj[0].push_back(1);
    adj[1].push_back(0);
    adj[1].push_back(2);
    adj[2].push_back(1);

    vector<vert_t> min_distance;

    Dijkstra(0, vertices, adj, min_distance);

    REQUIRE(min_distance[0] == 0);
    REQUIRE(min_distance[1] == 1);
    REQUIRE(min_distance[2] == 2);
}

TEST_CASE("cycled graph") {
    // 0 - 1
    // | /
    // 2

    unsigned int vertices = 3;

    vector<vector<vert_t>> adj(vertices);
    adj[0].push_back(1);
    adj[0].push_back(2);
    adj[1].push_back(0);
    adj[1].push_back(2);
    adj[2].push_back(0);
    adj[2].push_back(1);

    vector<vert_t> min_distance;

    Dijkstra(0, vertices, adj, min_distance);

    REQUIRE(min_distance[0] == 0);
    REQUIRE(min_distance[1] == 1);
    REQUIRE(min_distance[2] == 1);
}
