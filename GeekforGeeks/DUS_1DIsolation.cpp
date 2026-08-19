#include <bits/stdc++.h>
#define all(x) x.begin(),x.end()
#define F first
#define S second

using namespace std;
typedef long long ll;
typedef vector<int> vi;
typedef pair<int,int> pii;


const int MAX = 1e5 + 5;
int id[MAX];
int sz[MAX];

//multiset<int> sett;
vector< vector< pii > > adj(MAX);
vector< pii > nodes;
bool visited[MAX];


void initialize(int n){
    //sett.clear();
    adj.clear();
    nodes.clear(); 
    memset(visited, false, sizeof visited);
    
    for(int i = 1;i <=n;++i){   
         id[i]=i;
         sz[i]=1; //sett.insert(1);
     }
}
int findset(int x){
    while(id[x] != x){
        id[x] = id[id[x]];
        x = id[x];
    }
    return x;
}
 
void combine(int x, int y){
    int p = findset(x), q = findset(y);
    if(p==q) return;
    if(sz[q] >sz[p]) swap(p,q);
    
        //sett.erase( sett.find(sz[p])); sett.erase( sett.find(sz[q]));
        id[q]=p; sz[p]+=sz[q];

        sz[q]=-1;
        //sett.insert(sz[p]);
}
void dfs(int v,int W){
    visited[v] = 1;
    for(auto e: adj[v]){
        if(!visited[e.F]){
            nodes.push_back({e.S+W, e.F}); dfs(e.F, e.S+W);
        }
    }
}

int main(){
    
    int t; scanf("%d",&t);
    while(t-- > 0){
        int n,l; scanf("%d %d",&n,&l);
        
        initialize(n);
        int x,y,w; int u,v,W;
        
        scanf("%d %d %d",&u,&v,&W);
        
        
        for(int i=1;i<n;i++){
            scanf("%d %d %d",&x,&y,&w);
            if(abs(w) <= l) combine(x,y);

            adj[x].push_back({y,w});
            //adj[y].push_back({x,-1*w});
        }
        
        nodes.push_back({W,v});
        dfs(v,W);
        sort(all(nodes));
        // for(auto e: nodes)
        //     cout<< e.F <<" "<<e.S<<"\n";

        for(int i=1;i<nodes.size();i++){
            if(nodes[i].F - nodes[i-1].F <= l)
                combine(nodes[i].S, nodes[i-1].S);
        }

        int q,m; scanf("%d",&q);
        while(q-- > 0){
            scanf("%d",&m);
            int city[m];
            for(int i=0;i<m;i++) scanf("%d",city+i);

            map<int,int> G;
            for(int i=0;i<m;i++)
                G[ findset(city[i]) ]++;

            if(G.size() == 1) printf("YES\n");
            else{
                bool f = false;
                for(auto e: G){
                    if(e.S == 1){
                        f = true;
                        break;
                    }
                }
                
                if(f) printf("NO\n");
                else printf("%d\n", (int)G.size());
            }
            //cout<< *sett.rbegin() - *sett.begin() <<"\n";
        }
    }
    return 0;
}