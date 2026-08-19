#include <iostream>
#include <limits.h>
#include <string.h>
#include <queue>
using namespace std;
 
#define V 2001
int graph[V][V];
int rGraph[V][V];
int N;

bool bfs(int s, int t, int parent[])
{
    bool visited[N];
    memset(visited, false, sizeof(visited));
    
    queue <int> q;
    q.push(s);
    visited[s] = true;
    parent[s] = -1;
 
    while (!q.empty())
    {
        int u = q.front();
        q.pop();
 
        for (int v=0; v< N; v++)
        {
            if (visited[v]==false && rGraph[u][v] > 0)
            {
                q.push(v);
                parent[v] = u;
                visited[v] = true;
            }
        }
    }
    return (visited[t] == true);
}
// http://goo.gl/wtQ4Ks
int findDisjointPaths(int s, int t, int K)
{
    int u, v;
 
    memset(rGraph, 0, sizeof rGraph);
    for (u = 0; u < N; u++)
        for (v = 0; v < N; v++)
             rGraph[u][v] = graph[u][v];
 
    int parent[N]; 
    int max_flow = 0;
    while (bfs(s, t, parent))
    {
        int path_flow = INT_MAX;
 
        for (v=t; v!=s; v=parent[v])
        {
            u = parent[v];
            path_flow = min(path_flow, rGraph[u][v]);
        }
        for (v=t; v != s; v=parent[v])
        {   
            u = parent[v];
            rGraph[u][v] -= path_flow;
            rGraph[v][u] += path_flow;
        }
        max_flow += path_flow;

        if(max_flow >= K) return max_flow;
    }
    return max_flow;
}
int main()
{
    int t; scanf("%d",&t);

    int n,m,u,v;
    int S,D,K;

    while(t-- > 0){
        memset(graph, 0, sizeof graph);

        scanf("%d %d",&n,&m);
        N = n+1;
        
        for(int i=0;i < m;i++){
            scanf("%d %d",&u,&v);   graph[u][v] += 1;
        }
        scanf("%d %d %d",&S,&D,&K);
        if( findDisjointPaths(S, D , K) >= K) printf("YES\n"); else printf("NO\n");
    }
    return 0;
}
