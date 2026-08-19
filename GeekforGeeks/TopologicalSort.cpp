#include<bits/stdc++.h>
using namespace std;
 

 set< string > store;
 set<string>::iterator it;
 int ar[100010]={0},dex=0;
// Class to represent a graph
class Graph
{
    int V;    // No. of vertices'
 
    // Pointer to an array containing adjacency listsList
    list<int> *adj;
 
    // A function used by topologicalSort
    void topologicalSortUtil(int v, bool visited[], stack<int> &Stack);
public:
    Graph(int V);   // Constructor
 
     // function to add an edge to graph
    void addEdge(int v, int w);
    // prints a Topological Sort of the complete graph
    void topologicalSort();
};
 
Graph::Graph(int V)
{
    this->V = V;
    adj = new list<int>[V];
}
 
void Graph::addEdge(int v, int w)
{
    adj[v].push_back(w); // Add w to v’s list.
}
 
// A recursive function used by topologicalSort
void Graph::topologicalSortUtil(int v, bool visited[], 
                                stack<int> &Stack)
{
    // Mark the current node as visited.
    visited[v] = true;
 
    // Recur for all the vertices adjacent to this vertex
    list<int>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i)
        if (!visited[*i])
            topologicalSortUtil(*i, visited, Stack);
 
    // Push current vertex to stack which stores result
    Stack.push(v);
}
 
// The function to do Topological Sort. It uses recursive 
// topologicalSortUtil()
void Graph::topologicalSort()
{
    stack<int> Stack;
 
    // Mark all the vertices as not visited
    bool *visited = new bool[V];
    for (int i = 0; i < V; i++)
        visited[i] = false;
 
    // Call the recursive helper function to store Topological
    // Sort starting from all vertices one by one
    for (int i = 0; i < V; i++)
      if (visited[i] == false)
        topologicalSortUtil(i, visited, Stack);
 
    // Print contents of stack
    while (Stack.empty() == false)
    {

        ar[dex] = Stack.top();
        dex++;
        Stack.pop();
    }
}
 
// Driver program to test above functions
vector< pair<string,string> > v;
int main()
{
    // Create a graph given in the above diagram
    
    int n,m;
    string source,dest;
    cin>>n>>m;
    
    Graph g(n);
    for(int i=0;i<m;i++){
        cin>>source>>dest;
        
        v.push_back({source,dest});
        
        store.insert(source);
        store.insert(dest);

    }
    int ind0,ind1;
    for(int i=0;i<m;i++){
        ind0 = distance(store.begin(),store.find(v[i].first));
        ind1 = distance(store.begin(),store.find(v[i].second));
        g.addEdge(ind0,ind1);
//        cout<<ind0<<" "<<ind1<<endl;
    }
    int i=0;
/*    for(it = store.begin(); it!=store.end();it++){
        cout<<*it<<" "<<i++<<" ";
    }
    cout<<endl;
 */   
    g.topologicalSort();
    
    cout<<*(next(store.begin(),ar[n-1]));
    for(int i=n-2;i>=0;i--){
       cout<<" -> "<<*(next(store.begin(),ar[i]));
    }
 
    return 0;
}