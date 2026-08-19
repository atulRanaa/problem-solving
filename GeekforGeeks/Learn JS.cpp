# include <bits/stdc++.h>
# define inf 1e15
using namespace std;


struct edge {
    int v;
    long long w;
};

bool operator <(edge a, edge b) {
    return a.w < b.w;
}

void bfs(int source, int dest, vector< vector< edge > > &adj, vector< long long > &dist) {
    priority_queue< edge > Q;
    Q.push( (edge){source, 0} );
    dist[ source ] = 0;
    while(!Q.empty()) {
        edge moveto = Q.top();
        Q.pop();

        int from = moveto.v;
        for(auto &to: adj[ from ]) {

            if(to.w + dist[from] < dist[to.v]) {
                dist[to.v] = to.w + dist[from];
                Q.push(to);
            }
        }
    }
}

int main() {

    int n; cin >> n;
    int ar[n];
    unordered_map < int, int> ids;

    int idx = 1;
    for(int i = 0; i < n; i++) {
        cin >> ar[i];

        ids[ ar[i] ] = idx++;
    }

    vector< vector< edge > > adj(idx + 10);
    vector< long long > dist(idx + 10);
    for(int i = 0; i < idx + 10; i++) {
        dist[i] = inf;
        adj[i].clear();
    }

    int e; cin >> e;
    int u, v;
    long long w;
    for(int i = 0; i < e; i++) {
        cin >> u >> v >> w;
        int to = ids[u];
        int fo = ids[v];

        adj[ to ].push_back( (edge){fo, w} );
    }

    int source, dest;
    cin >> source >> dest;

    bfs(ids[source], ids[dest], adj, dist);
    // for(int i = 0; i < n; i++) {
    //     cout << ar[i] << " " << ids[ ar[i] ] << " " << dist[ ids[ ar[i] ] ] << "\n";
    // }
    // cout << (int)dfs(ids[source], -1, ids[dest], adj, visited) << "\n";
    if(dist[ ids[dest] ] == (long long)inf)
        cout << -1 << "\n";
    else
        cout << dist[ ids[dest] ] << "\n";

    return 0;
}