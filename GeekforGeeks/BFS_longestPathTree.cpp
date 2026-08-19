#include<bits/stdc++.h>
using namespace std;

vector<int> adj[100005];
bool visited[100005];
int V;

pair<int, int> bfs(int u)
{
    int dis[100005];
    int maxDis = 0;
    int nodeIdx= u;
    memset(dis, -1, sizeof(dis));
 
    queue<int> q;
    q.push(u);
    dis[u] = 0;
    visited[u] = true;
    while (!q.empty()){

        int t = q.front();       q.pop();       
        for (int it = 0; it < adj[t].size(); it++)
        {
            int v = adj[t][it];
            if (dis[v] == -1)
            {
                q.push(v);
                dis[v] = dis[t] + 1;
                visited[v] = true;
            }
            if (dis[v] > maxDis)
            {
                maxDis = dis[v];
                nodeIdx = v;
            }
        }
    }
    return make_pair(nodeIdx, maxDis);
}

int longestPathLength(int src)
{
    pair<int, int> t1, t2;
    t1 = bfs(src);
    t2 = bfs(t1.first);
 
    return t2.second;
}
int main(){
    int n,k;    cin>>n>>k;
    int ar[n];
    for(int i=1;i <= n; i++) cin>>ar[i];
    V = n;
    int x,y;
    for(int i=1;i<=n-1;i++){
        cin>>x>>y;
        if( ar[x]%k == 0 && ar[y]%k == 0){
            adj[x].push_back(y);
            adj[y].push_back(x);
        }
    }
    memset(visited, false, sizeof(visited));
    int ans = 0;
    for(int i=1; i<=n;i++){
        if(ar[i]%k == 0 && !visited[i]){
            ans = max(ans, longestPathLength(i) );
        }
    }
    cout<<ans;
    return 0;
}