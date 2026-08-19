#include "bits/stdc++.h"
using namespace std;
#define sz 100001
 
vector < int > adj[sz];    // stores the tree
vector < int > euler;      // tracks the eulerwalk
vector < int > depthArr;   // depth for each node corresponding
                           // to eulerwalk
 
int FAI[sz];     // stores first appearence index of every node
int level[sz];   // stores depth for all nodes in the tree
int ptr;         // pointer to euler walk
int dp[sz][18];  // sparse table
int logn[sz];    // stores log values
int p2[20];      // stores power of 2
 
void buildSparseTable(int n)
{
    memset(dp,-1,sizeof(dp)); // initializing sparse table
 
    // filling base case values
    for (int i=1; i<n; i++)
        dp[i-1][0] = (depthArr[i]>depthArr[i-1])?i-1:i;
 
    // dp to fill sparse table
    for (int l=1; l<18; l++)
      for (int i=0; i<n; i++)
        if (dp[i][l-1]!=-1 and dp[i+p2[l-1]][l-1]!=-1)
          dp[i][l] = (depthArr[dp[i][l-1]]>depthArr[dp[i+p2[l-1]][l-1]])? dp[i+p2[l-1]][l-1] : dp[i][l-1];
        else  
            break;
}
 
int query(int l,int r)
{
    int d = r-l;
    int dx = logn[d];
    if (l==r) return l;
    if (depthArr[dp[l][dx]] > depthArr[dp[r-p2[dx]][dx]]) return dp[r-p2[dx]][dx];
    else return dp[l][dx];
}
 
void preprocess()
{
    // memorizing powers of 2
    p2[0] = 1;
    for (int i=1; i<18; i++)
        p2[i] = p2[i-1]*2;
 
    // memorizing all log(n) values
    int val = 1,ptr=0;
    for (int i=1; i<sz; i++)
    {
        logn[i] = ptr-1;
        if (val==i)
        {
            val*=2;
            logn[i] = ptr;
            ptr++;
        }
    }
}
 
/**
 * Euler Walk ( preorder traversal)
 * converting tree to linear depthArray
 * Time Complexity : O(n)
 * */
void dfs(int cur,int prev,int dep)
{
    // marking FAI for cur node
    if (FAI[cur]==-1) FAI[cur] = ptr;
 
    level[cur] = dep;
    euler.push_back(cur); // pushing root to euler walk
    
    ptr++; // incrementing euler walk pointer
 
    for (auto x:adj[cur]){
        if (x != prev){
            dfs(x,cur,dep+1);
 
            // pushing cur again in backtrack of euler walk
            euler.push_back(cur);
            // increment euler walk pointer
            ptr++;
        }
    }
}
 
// Create Level depthArray corresponding to the Euler walk Array
void makeArr()
{
    for (auto x : euler)
        depthArr.push_back(level[x]);
}
 
int LCA(int u,int v)
{
    // trival case
    if (u==v)
       return u;
 
    if (FAI[u] > FAI[v])
       swap(u,v);
 
    // doing RMQ in the required range
    return euler[query(FAI[u], FAI[v])];
}
 
void addEdge(int u,int v)
{
    adj[u].push_back(v);
    adj[v].push_back(u);
}

int updateValue[sz]={0};
int updateAnc[sz]={0};
void dfs1(int cur, int prev){
    int sum = 0;

    for (auto x:adj[cur]){
        if (x != prev){
            dfs1(x,cur);

            sum += (updateValue[x] + updateAnc[x]);
        }
    }
    updateValue[cur] += sum;
}

int main(int argc, char const *argv[])
{
    // constructing the described tree
    int n,q,u,v; scanf("%d %d",&n,&q);
    for(int i=0;i<n-1;i++){
        scanf("%d %d",&u,&v);  addEdge(u,v);
    }
    
    preprocess(); // performing required precalculations
    ptr = 0;
    memset(FAI,-1,sizeof(FAI));
    dfs(1,0,0);     // doing the Euler walk
    makeArr();      // creating depthArray corresponding to euler[]
 
    buildSparseTable(depthArr.size()); // building sparse table
 
    for(int i=0;i<q;i++){
        scanf("%d %d",&u,&v);
        int anc = LCA(u,v);
        updateValue[u] += 1, updateValue[v] += 1;
        updateValue[ anc ] -= 1, updateAnc[ anc ] -= 1;
        cout<<LCA(u,v)<<"\n";
    }
    dfs1(1,0);

    for(int i=1;i <= n;i++) printf("%d ",updateValue[i]);
    return 0;
}
