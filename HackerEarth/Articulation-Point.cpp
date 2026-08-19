// hackerearth       
        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const ll N = 3e5+5;
        const ll MOD = 1000000007;
        #define all(x) x.begin(),x.end()
        #define pii pair<ll,ll>
        #define pis pair<ll,string>
        #define F first
        #define S second
        #define LCM(a,b) ((a*b)/__gcd(a,b))
        #define inf 1e15
        #define test ll cse;scanf("%lld",&cse);for(ll _i=1;_i<=cse;_i++)
        #define PI 3.14159265
        typedef vector<vector<ll>> matrix;
        typedef vector<ll> vi;
 
 
vector< int > graph[N];
bool used[N];
int timer, tin[N], fup[N], par[N];

int sizeOfNode[N];

int n,m;
set< int > Ap;
vector< pair<int,int> > Br;
void dfs(int v, int p = -1) {
    used[v] = true;
    tin[v] = fup[v] = timer++;
    int children = 0;
    int isArti = false;
    
    for (int to: graph[v]) {
        if (to == p)  continue;
        
        if (used[to])
            fup[v] = min (fup[v], tin[to]);
        else {
            par[to] = v;
            dfs (to, v);
            fup[v] = min (fup[v], fup[to]);
            if(fup[to] > tin[v]) {
                Br.push_back({ min(to,v), max(to,v)}); // bridge
            }
            if (fup[to] >= tin[v] && p != 0) {
                Ap.insert(v);             //v is articulation point
                isArti = true;
            }
            ++children;
            sizeOfNode[v] += 1;
         }
        
    }

    sizeOfNode[v] += 1;
    if ((p == 0 && children > 1) || (p!=0 && isArti)){
        Ap.insert(v);                    //v is articulation point
    }
 
}
 
int main() {
    memset(sizeOfNode, 0, sizeof sizeOfNode);

    scanf("%d %d",&n,&m);
    int x,y;
    for(int i=0;i<m;i++){
        scanf("%d %d",&x,&y);
        graph[x].push_back(y);
        graph[y].push_back(x);
    }
    timer = 1;
    
    dfs(0);
    
    for(int i=1;i<=n;i++) cout << tin[i] << " "; cout << "\n";
    for(int i=1;i<=n;i++) cout << fup[i] << " "; cout << "\n";
    for(int i=1;i<=n;i++) cout << sizeOfNode[i] << " "; cout << "\n";
    for(int e: Ap) cout<< e << " "; cout<< "\n";
    for(pair<int,int> e: Br) cout<< e.first << " " << e.second <<"\n";
    return 0;
}