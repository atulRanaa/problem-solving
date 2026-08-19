# include <bits/stdc++.h>
using namespace std;

bool dfs(int node, int dest, vector< vector<int> > &adj, vector< bool > &visited) {
    visited[node] = true;

    if(node == dest) return true;
    bool ans = false;
    for(int subNodes: adj[node]) {
        if(!visited[subNodes]) {
            ans |= dfs(subNodes, dest, adj, visited);
        }
    }
    return ans;
}

int main() {

    int n; cin >> n;
    int ar[n];
    unordered_map < int, int> ids;
    unordered_map < int, int> reverseIds;
    int idx = 1;
    for(int i = 0; i < n; i++) {
        cin >> ar[i];

        ids[ ar[i] ] = idx;
        reverseIds[ idx ] = ar[i];
        idx++;
    }

    vector< vector< int > > adj(idx + 10);
	vector< vector< int > > reverseAdj(idx + 10);
    vector< bool> visited(idx + 10);
    for(int i = 0; i < idx + 10; i++) {
        visited[i] = false;
        adj[i].clear();
		reverseAdj[i].clear();
    }

    int e; cin >> e;
    int u, v;
    for(int i = 0; i < e; i++) {
        cin >> u >> v;
        int to = ids[u];
        int fo = ids[v];
        adj[ to ].push_back( fo );
        reverseAdj[ fo ].push_back( to );
    }

    int source, dest;
    cin >> source >> dest; // A, B
    bool rechable = dfs(ids[source], ids[dest], reverseAdj, visited);

    vector< int > ans;
    if(rechable) {
        for(auto to: reverseAdj[ ids[dest] ]) {
            if(visited[ to ]) ans.push_back( reverseIds[ to ] );
        }
        sort(ans.begin(), ans.end());
        for(auto rev: ans) cout << rev << " "; cout << "\n";
    }
    return 0;
}