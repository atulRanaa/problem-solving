// hackerearth
        #include <bits/stdc++.h>
        #define all(x) x.begin(),x.end()
        #define F first
        #define S second
        using namespace std;
        typedef long long ll;
        typedef vector<int> vi;
        typedef pair<int,int> pii;


const int MAX = 1e4 + 5;
int id[MAX];
int sz[MAX];

//multiset<int> sett;
//vector< vector< pii > > adj(MAX);
vector< pair< int,pii>> nodes;
bool visited[MAX];


void initialize(int n){
    //sett.clear();
    //adj.clear();
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
ll sum=0;
void combine(int i,int x, int y){
    int p = findset(x), q = findset(y);
    if(p==q) return;
    sum += nodes[i].F;
    if(sz[q] >sz[p]) swap(p,q);
    
        //sett.erase( sett.find(sz[p])); sett.erase( sett.find(sz[q]));
        id[q]=p; sz[p]+=sz[q];

        sz[q]=-1;
        //sett.insert(sz[p]);
}

int main(){
    
        int n,m; scanf("%d %d",&n,&m);
        
        initialize(n);
        int x,y,W;
        
        for(int i=1;i<=m;i++){
            scanf("%d %d %d",&x,&y,&W);
            nodes.push_back({W,{x,y}});
        }
        sort(all(nodes));
        for(int i=0;i<m;i++)
            combine(i,nodes[i].S.F,nodes[i].S.S);
        printf("%lld",sum);
    return 0;
}