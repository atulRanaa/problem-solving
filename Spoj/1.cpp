// spoj    
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
        #define test ll cse;cin>>cse;for(ll _i=1;_i<=cse;_i++)
        #define PI 3.14159265
        #define fast ios_base::sync_with_stdio(false);cin.tie(NULL);
        const double EPS = 1E-9;
        typedef vector<vector<double>> matrix;
        typedef vector<int> vi;


int main(){
    fast;
    int n,q ; cin>>n>>q;

    ll ar[n+1]={0};

    ll h[n+1][66];
    memset(h, 0, sizeof h);
    for(int i=1;i<=n;i++) cin>>ar[i];

    for(int i=1;i<=n;i++){

        for(ll j=0;j<34;j++){
                if((1<<j)&ar[i]) h[i][j]=1;
        }
    }
    // for(int i=0;i<=n;i++){
    //     for(int j=0;j<20;j++) cout<<h[i][j]<<" ";
    //     cout<<"\n";
    // }
    for(int i=1;i<=n;i++){
        for(int j=0;j<34;j++)
            h[i][j] += h[i-1][j];
    }
    // cout<<"\n";
    // for(int i=0;i<=n;i++){
    //     for(int j=0;j<20;j++) cout<<h[i][j]<<" ";
    //     cout<<"\n";
    // }
    int l,r; ll x;
    for(int i=0;i<q;i++){
        cin >>l>>r>>x;
        
        ll sum = 0;
        for(ll j=0;j<34;j++){

                if((1<<j)&x){
                    //cout<<1<<" ";
                    sum += (1<<j)*(r-l+1 - (h[r][j]-h[l-1][j]));
                }
                else{
                    //cout<<0<<" ";
                    sum += (1<<j)*( h[r][j]-h[l-1][j]);
                }
        }
        // cout<<"\n";
        // for(int j=0;j<20;j++) cout<<h[r][j]-h[l-1][j]<<""
        cout<< sum <<"\n";
    }
}