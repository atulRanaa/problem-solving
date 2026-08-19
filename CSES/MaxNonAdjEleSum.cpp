
        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const int N = 1e5+5;
        #define all(x) x.begin(),x.end()
        #define MOD 1000000007
        #define pii pair<int,int>
        #define pis pair<int,string>
        #define F first
        #define S second
        #define LCM(a,b) ((a*b)/__gcd(a,b))
        #define inf 1e15
        #define test ll cse;cin>>cse;for(ll i=1;i<=cse;i++)
        #define fast ios_base::sync_with_stdio(false);cin.tie(NULL);
        #define PI 3.14159265
        #define MAXN 1000001

int main(){
    
    fast;    
    test{
        ll n; cin>>n;
        ll ar[n]; for(int i=0;i<n;i++) cin>>ar[i];


        ll dp[n];
        ll pt[n];
        memset(dp,0 ,sizeof(dp));
        memset(pt,-1,sizeof(pt));



        ll inc,x=-1, exc,y=-1;

        inc = ar[0];
        exc = 0;

        dp[0] = inc;
        pt[0] = -1;

        x = 0;
        for(int i=1;i<n;i++){

            ll tmp = inc, ind = x;

            //inc = max( inc, exc + ar[i]);

            if(exc + ar[i] > inc){
                inc = exc + ar[i];
                pt[i] = y;
                x = i;
            }
            exc = tmp, y = ind; 
        }

        cout << max(inc,exc) <<"\n";
        for(int i=0;i<n;i++) cout<<pt[i]<<" "; cout<<"\n";
    }
    return 0;
}