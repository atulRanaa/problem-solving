    #include <bits/stdc++.h>
         
    using namespace std;
     
    typedef long long ll; 
    const int N = 1e6+5; 
    #define MOD 1000000007
    #define pii pair<int,int>
    #define pis pair<int,string>
    #define F first
    #define S second
    #define LCM(a,b) ((a*b)/__gcd(a,b))
    #define inf 1e10
    ll ar[N];
    
    vector<int> adj[N];
    int color[N];
    
    
    void dfs(int p,int P, int anc[]){
        
        ar[p] = (anc[ color[p] ] != 0)?(anc[ color[p] ]):(-1);
        int temp = anc[ color[p] ];
        anc[ color[p] ] = p;
        for(int c:adj[p]){
            if(c != P) dfs(c,p,anc);
        }
        anc[ color[p] ] = temp;
    }
    int main(){
        ios_base::sync_with_stdio(false);
        cin.tie(NULL);
        
        
        int n,c,m; cin>>n>>c;
        for(int i=2;i<=n;i++){
            cin>>m;
            adj[i].push_back(m); adj[m].push_back(i);
        }
        for(int i=1;i<=n;i++) cin>>color[i];
        int anc[N];
        memset(anc, 0, sizeof anc);
        dfs(1, 0,anc);
        /*for(int i=1;i<=n;i++){
            for(int c:adj[i]) cout<<c<<" "; cout<<"\n";
        }*/
        for(int i=1;i<=n;i++) cout<<ar[i]<<" ";
        return 0;
    }