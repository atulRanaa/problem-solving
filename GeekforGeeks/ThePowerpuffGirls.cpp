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
    ll n; cin >> n;
    ll q[n]; for(ll i=0;i<n;i++) cin >> q[i];
    ll Q;
    ll ans = LLONG_MAX;
    for(ll i=0;i<n;i++) {
        cin >> Q;
        ans = min(ans, Q/q[i]);
    }
    cout << ans << "\n";
}