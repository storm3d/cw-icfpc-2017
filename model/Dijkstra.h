#ifndef CW_ICFPC_2017_DIJKSTRA_H
#define CW_ICFPC_2017_DIJKSTRA_H

#include <vector>
#include "GameState.h"

void Dijkstra(vert_t s, vert_t n, std::vector<std::vector<vert_t>> &adj, std::vector<vert_t> &dis);

#endif //CW_ICFPC_2017_DIJKSTRA_H
