#include <cassert>
#include "Empire.h"

Empire::Empire(const GameState& game_, Dijkstra& pathfinder_, punter_t punter)
        : game(game_),
          pathfinder(pathfinder_),
          id(punter)
{
}

long Empire::getScore(vert_t from, vert_t to) {
//    return pathfinder.getShortestPathLength(from, to);
    return 1;
}
const std::vector<Empire::Path> Empire::getPaths() const
{
    std::vector<Empire::Path> paths;
    return paths;
}