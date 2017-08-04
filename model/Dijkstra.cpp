//Implementation for Dijkstra's SSSP(Single source shortest path) algorithm
//This is an optimized algorithm running in O(E*log(V))
// See https://www.quora.com/What-is-the-most-simple-efficient-C%2B%2B-code-for-Dijkstras-shortest-path-algorithm/answer/Shreyans-Sheth-1?srid=3Bso
// and https://github.com/bholagabbar/algorithmic-programming/blob/master/Code/WorkspaceB/C%2B%2B_Files/DS_ALGO/GRAPH%20ALGOS/DIJKSTRA/DIJKSTRA_USE.cpp

// This code is slightly modified as edge weight is always 1 in our case


#include <queue>

using namespace std;

#include "Dijkstra.h"

#define pii pair<vert_t,vert_t>
#define F first
#define S second
#define mp make_pair
#define pb emplace_back

//bool vis[100001]; //visited array
//int dis[100001]; //storing shortest distance from source to every vertex
//vector<int> adj[100001]; //Adjacency List. source vertex->destination vertex

class prioritize {
public: bool operator ()(pii &p1 , pii &p2) {
        return p1.S > p2.S;
    }
};

//s - source vertex
//n - number of vertexes
//vis - visited array
//dis - storing shortest distance from source to every vertex
//adj - Adjacency List. source vertex->destination vertex
void Dijkstra(vert_t s, vert_t n, vector<vector<vert_t>> adj, vert_t dis[]) {
    vector<bool> vis(n);
    for (int i = 0; i <= n; i++) {
        vis[i] = false;
        dis[i] = INT_MAX;
    }
    priority_queue<pii, vector<pii>, prioritize> pq;
    pq.push(mp(s, dis[s] = 0));
    while (!pq.empty()) {
        pii cur = pq.top(); pq.pop();
        vert_t cv = cur.F, cw = cur.S;
        if (vis[cv]) continue;
        vis[cv] = true;
        for (vert_t x : adj[cv]) {
            if (!vis[x] && (cw + 1) < dis[x]) {
                pq.push(mp(x, dis[x] = cw + 1));
            }
        }
    }
}
