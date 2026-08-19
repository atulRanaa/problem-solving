#include <bits/stdc++.h>

using namespace std;

vector<string> split_string(string);

int comp;
void dfs(int s, vector<int> adj[], int visited[]){
    visited[s] = 1; comp += 1;
    for(auto ele: adj[s]) {
        if(visited[ele] == 0) dfs(ele, adj, visited);
    }
}
long roadsAndLibraries(int n, int m, int c_lib, int c_road, vector<vector<int>> cities){
    vector<int> adj[n+1]; int visited[n+1]={0};

    if(m == 0) return (long)c_lib * (long) n;
    for(auto ele: cities)
        adj[ ele[0]].push_back(ele[1]), adj[ ele[1]].push_back(ele[0]);
    
    if(c_lib <= c_road) return ((long)n * (long)c_lib);
    long ans = 0;
    for(int i=1; i<=n; i++){
        comp = 0;
        if(visited[i] == 0){ dfs(i, adj, visited);
            ans += (comp-1)*(long)c_road + (long)c_lib;
        }
    }
    return ans;
}

int main()
{

    int q;
    cin >> q;
   
    for (int q_itr = 0; q_itr < q; q_itr++) {
        int n, m, c_lib, c_road; cin>>n>>m>>c_lib>>c_road;

        vector< vector<int> > cities(m);
        for (int i = 0; i < m; i++) {
            cities[i].resize(2);

            for (int j = 0; j < 2; j++) {
                cin >> cities[i][j];
            }
        }

        long result = roadsAndLibraries(n, m, c_lib, c_road, cities);
        cout<<result<<"\n";
    }
    return 0;
}
