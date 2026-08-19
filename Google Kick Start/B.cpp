#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll N = 3e5 + 5;
const ll MOD = 1e9 + 7;
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
typedef vector< vector<double> > matrix;
typedef vector< vector<ll> > array_2D;
typedef pair<ll, pair<ll, ll> > ppi;
typedef vector<ll> vi;

bool comp(const pair<int, string> &a, const pair<int, string> &b) {
    return  (a.first > b.first) ||
            (a.first == b.first && a.second < b.second);
}



ll ar[N], prefixSum[N];
unordered_map<int, bool> prefectSquare;
int main() {
    fast;

    ll n, k;
    for(int i = 0; i*i < MOD; i++) prefectSquare[i*i] = true;
    test {
        cin >> n;
        for(int i = 1; i <= n; i++) cin >> ar[i];

        prefixSum[0] = 0;
        for(int i = 1; i <= n; i++) prefixSum[i] = ar[i] + prefixSum[i-1];


        long long ans = 0;
        for(int i = 1; i <= n; i++) {
            for(int j = i; j <= n; j++) {
                if(prefectSquare.find(prefixSum[j] - prefixSum[i-1]) != prefectSquare.end())
                    ans++;
            }
        }

        cout << "Case #" << _i << ": " << ans << "\n";
    }
    return 0;
}