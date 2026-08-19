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

bool within(int i, int j, int n, int m) {
    return i >= 0 && j >= 0 && i < n && j < m;
}

int main() {
    fast;

    ll n; cin >> n;
    vector<ll> dp(n + 5, 0);

    dp[0] = 1;
    // dp[1] = 1;
    // dp[2] = dp[0] + dp[1];
    // dp[3] = dp[0] + dp[1] + dp[2];

    for(int i = 1; i <= n; i++) {
        for(int k = 1; k <= 6; k++) {
            if(i - k >= 0)
                dp[i] += dp[i-k];

            dp[i] %= MOD;
        }
    }

    cout << dp[n] << "\n";
    
    return 0;
}