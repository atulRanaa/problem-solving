// spoj    
        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const ll N = 1e5+5;
        const ll MOD = 1000000007;
        #define all(x) x.begin(),x.end()
        #define pii pair<ll,ll>
        #define pis pair<ll,string>
        #define F first
        #define S second
        #define LCM(a,b) ((a*b)/__gcd(a,b))
        #define inf 1e15
        #define test ll cse;cin>>cse;for(ll _i=1;_i<=cse;_i++)
        #define PI 3.14159265
        #define fast ios_base::sync_with_stdio(false);cin.tie(NULL);
        const double EPS = 1E-9;
        typedef vector<vector<double>> matrix;
        typedef vector<int> vi;

vector<int> adj[N];
int ans[N];
int vis[N];
int n,k;
typedef struct
{
    int w;
    map<int,int> H;
}node;

node A[N];
void dfs(int v, int par=0){
    int W = A[v].w;
    vis[v]=1;
    for(int u: adj[v]){
        if(!vis[u]){
            dfs(u,v);

            for(auto e: A[u].H){
                //cout<< it->F <<" "<<it->S<<"\n";
                A[v].H[ e.F ] += e.S;
                if((W^e.F)==k) ans[v]+=e.S;
            }
        }
    }
    //cout<<W<<" ";
    A[v].H[W]+=1;

    //for(auto e: A[v].H) cout<<e.F<<" "<<e.S<<"\n"; cout<<"\n";
}
int main(){
    fast;
    memset(ans, 0, sizeof ans);
    memset(vis, 0, sizeof vis);
    cin>>n>>k;
    int x,y,W;
    for(int i=1;i<=n;i++){
        cin>>W; A[i].w = W;
        //cout<<A[i].w<<" ";
    }
    for(int i=0;i<n-1;i++){
        cin>>x>>y;
        //cout<<x<<" "<<y<<"\n";
        adj[x].push_back(y),adj[y].push_back(x);
    }
    dfs(1);
    for(int i=1;i<=n;i++) cout<<ans[i]<<" "; cout<<"\n";
    return 0;
}