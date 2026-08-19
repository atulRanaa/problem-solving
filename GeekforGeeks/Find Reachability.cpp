# include <bits/stdc++.h>
using namespace std;

bool dfs(int node, int par, int dest, vector< vector<int> > &adj, vector< bool > &visited) {
    visited[node] = true;

    if(node == dest) return true;
    bool ans = false;
    for(int subNodes: adj[node]) {
        if(!visited[subNodes]) {
            ans |= dfs(subNodes, node, dest, adj, visited);
        }
    }
    return ans;
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

    vector< vector< int > > adj(idx + 10);
    vector< bool> visited(idx + 10);
    for(int i = 0; i < idx + 10; i++) {
        visited[i] = false;
        adj[i].clear();
    }

    int e; cin >> e;
    int u, v;
    for(int i = 0; i < e; i++) {
        cin >> u >> v;
        int to = ids[u];
        int fo = ids[v];
        adj[ to ].push_back( fo );
        // adj[ fo ].push_back( to );
    }

    int source, dest;
    cin >> source >> dest;
    cout << (int)dfs(ids[source], -1, ids[dest], adj, visited) << "\n";

    return 0;
}
