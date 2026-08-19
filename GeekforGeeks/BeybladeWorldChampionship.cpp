#include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const ll N = 1e4+5;
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
        #define fast ios_base::sync_with_stdio(false);cin.tie(NULL);cout.tie(NULL);
        const double EPS = 1E-9;
        typedef vector<vector<double>> matrix;
        typedef vector<int> vi;


int main() {
    fast;
    test { 
        ll n; cin >> n;
        ll G[n]; for(ll i=0;i<n;i++) cin >> G[i];
        ll O[n]; for(ll i=0;i<n;i++) cin >> O[i];

        sort(G, G+n);
        sort(O, O+n);

        ll ans = 0;
        int x = n-1, y = n-1;

        for(ll p: G) cout << p << " "; cout << "\n";
        for(ll p: O) cout << p << " "; cout << "\n";
        
        while(x >= 0 && y >= 0) {
            if(G[x] > O[y]) {
                x--;
                y--;
                ans++;
            }
            else // G[x] <= O[y]
                y--;
        }
        cout << ans << "\n";
    }
    return 0;
}