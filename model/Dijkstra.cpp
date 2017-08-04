//Implementation for Dijkstra's SSSP(Single source shortest path) algorithm
//This is an optimized algorithm running in O(E*log(V))
// See https://www.quora.com/What-is-the-most-simple-efficient-C%2B%2B-code-for-Dijkstras-shortest-path-algorithm/answer/Shreyans-Sheth-1?srid=3Bso
// and https://github.com/bholagabbar/algorithmic-programming/blob/master/Code/WorkspaceB/C%2B%2B_Files/DS_ALGO/GRAPH%20ALGOS/DIJKSTRA/DIJKSTRA_USE.cpp

// This code is slightly modified as edge weight is always 1 in our case


#include <bits/stdc++.h>
using namespace std;
#define endl '\n'
#define pii pair<int,int>
#define F first
#define S second
#define mp make_pair
#define pb emplace_back

bool vis[100001];
int dis[100001];
vector<pii> a[100001];

class prioritize {
public: bool operator ()(pii &p1 , pii &p2) {
        return p1.S > p2.S;
    }
};

int Dijkstra(int s, int n) {
    for (int i = 0; i <= n; i++) {
        vis[i] = false;
        dis[i] = INT_MAX;
    }
    priority_queue<pii, vector<pii>, prioritize> pq;
    pq.push(mp(s, dis[s] = 0));
    while (!pq.empty()) {
        pii cur = pq.top(); pq.pop();
        int cv = cur.F, cw = cur.S;
        if (vis[cv]) continue;
        vis[cv] = true;
        for (pii x : a[cv]) {
            if (!vis[x.F] && (cw + x.S) < dis[x.F]) {
                pq.push(mp(x.F, dis[x.F] = cw + x.S));
            }
        }
    }
}
