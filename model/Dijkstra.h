#ifndef CW_ICFPC_2017_DIJKSTRA_H
#define CW_ICFPC_2017_DIJKSTRA_H

#include <vector>
#include <memory>

typedef uint64_t vert_t;

void Dijkstra(vert_t s, std::vector<std::vector<vert_t>> &adj, std::vector<vert_t> &dis);

#endif //CW_ICFPC_2017_DIJKSTRA_H
