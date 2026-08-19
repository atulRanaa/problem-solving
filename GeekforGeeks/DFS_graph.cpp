
        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const int N = 4*1e5+5;
        #define all(x) x.begin(),x.end()
        #define MOD 1000000007
        #define pii pair<int,int>
        #define pis pair<int,string>
        #define F first
        #define S second
        #define LCM(a,b) ((a*b)/__gcd(a,b))
        #define inf 1e15
        #define test ll cse;cin>>cse;for(ll i=1;i<=cse;i++)
        #define fast ios_base::sync_with_stdio(0);cin.tie(NULL);cout.tie(NULL);
        #define PI 3.14159265
        #define MAXN 10000001
        #define pb      push_back
        #define mp      make_pair
        
        vector<int> G[N];
        bool used[N];

vector<int> ar;
void dfs(int u){
	ar.pb(u);
        used[u]=true;

        for(int e: G[u]){
                if(!used[e]) dfs(e);
        }

        if(G[u].size() > 1) ar.pb(u);
}
int main(){
        fast;
        int n,m;  cin>>n>>m;
        
        int x,y;
        for(int i=1;i<=m;i++){       
                cin>>x>>y;
                G[x].pb(y);
                G[y].pb(x);
        }
        
        memset( used , false, sizeof(used));        
        dfs(1);

        for(int e: ar) cout << e<<" "; cout <<"\n";
        return 0;
}