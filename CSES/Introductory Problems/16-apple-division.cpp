#include <climits>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>

using namespace std;
typedef long long ll;
const ll N = 2e5 + 5;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define LCM(a,b) ((a*b)/__gcd(a,b))
#define inf 1e15
#define test ll cse;cin>>cse;for(ll _i=1;_i<=cse;_i++)
#define PI 3.14159265
#define fast ios_base::sync_with_stdio(false);cin.tie(NULL);cout.tie(NULL);
const double EPS = 1E-9;
typedef vector< vector<double> > matrix;
typedef vector<int> vi;

template <typename T>
void inspect(const char* name, const T& container) {
    std::cerr << "[INSPECT] " << name << " = [ ";
    bool first = true;
    for (const auto& item : container) {
        if (!first) std::cerr << ", ";
        std::cerr << item;
        first = false;
    }
    std::cerr << " ]\n";
}
#define inspect(x) inspect(#x, x)

ll binpow(ll a, ll b, ll m) {
    a %= m;
    ll res = 1;
    while (b > 0) {
        if (b & 1)
            res = res * a % m;
        a = a * a % m;
        b >>= 1;
    }
    return res;
}

ll ans = INT_MAX;
void solve(int i, ll sum1, ll sum2, ll &n, vector<ll> &ar) {
    if(i == n) {
        ans = min(ans, abs(sum1 - sum2));
        return;
    }

    solve(i + 1, sum1 + ar[i], sum2, n, ar);
    solve(i + 1, sum1, sum2 + ar[i], n, ar);
}

int main() {
    fast;

    ll n; cin >> n;

    vector<ll> ar(n);
    for(int i = 0; i < n; i++) {
        cin >> ar[i];
    }

    solve(0, 0LL, 0LL, n, ar);
    cout << ans << "\n";

    return 0;
}