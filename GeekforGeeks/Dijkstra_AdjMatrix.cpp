// A C / C++ program for Dijkstra's single source shortest path algorithm.
// The program is for adjacency matrix representation of the graph
#include <bits/stdc++.h>
using namespace std;
  
vector<pair<long int,long int> >vec;
  
vector<pair<long int,long int> >::iterator it;
// Number of vertices in the graph
int V;
long int graph[10001][10001]={0};
// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
int minDistance(long int dist[], bool sptSet[])
{
   // Initialize min value
   long int min = INT_MAX, min_index;
  
   for (int v = 0; v < V; v++)
     if (sptSet[v] == false && dist[v] <= min)
         min = dist[v], min_index = v;
  
   return min_index;
}
  
// A utility function to print the constructed distance array

/*int printSolution(long int dist[], int n)
{
   for (int i = 0; i < V; i++)
      printf("%d \t %d\n", i, dist[i]);
}
*/
// Funtion that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
void dijkstra(int src)
{
     long int dist[V];     // The output array.  dist[i] will hold the shortest
                      // distance from src to i
  
     bool sptSet[V]; // sptSet[i] will true if vertex i is included in shortest
                     // path tree or shortest distance from src to i is finalized
  
     // Initialize all distances as INFINITE and stpSet[] as false
     for (int i = 0; i < V; i++)
        dist[i] = INT_MAX, sptSet[i] = false;
  
     // Distance of source vertex from itself is always 0
     dist[src] = 0;
  
     // Find shortest path for all vertices
     for (int count = 0; count < V-1; count++)
     {
       // Pick the minimum distance vertex from the set of vertices not
       // yet processed. u is always equal to src in first iteration.
       int u = minDistance(dist, sptSet);
  
       // Mark the picked vertex as processed
       sptSet[u] = true;
  
       // Update dist value of the adjacent vertices of the picked vertex.
       for (int v = 0; v < V; v++)
  
         // Update dist[v] only if is not in sptSet, there is an edge from 
         // u to v, and total weight of path from src to  v through u is 
         // smaller than current value of dist[v]
         if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX 
                                       && dist[u]+graph[u][v] < dist[v])
            dist[v] = dist[u] + graph[u][v];
     }
  
     // print the constructed distance array
     //printSolution(dist, V);

     for (int i=1;i<V;i++)
     {
       vec.push_back(make_pair(dist[i],i));
     }
     sort(vec.begin(),vec.end());
}
  
// driver program to test above function
int main()
{
   /* Let us create the example graph discussed above */
    long int n,s;
    long int ans=0;
    cin>>n>>s;
    int u,v,d;

    V = n+1;
    for(int i=0;i<n;i++){
      cin>>u;
      if(u == 0 ) break;
      cin>>v>>d;
      graph[u][v]=d;
      graph[v][u]=d;
    }
    dijkstra(s);
  
    for (it = vec.begin(); it != vec.end(); ++it){
      cout<<it->second<<" ";
      ans = (ans + it->first)%1000000007;
    }
    cout<<"\n"<<ans;
    return 0;
}
