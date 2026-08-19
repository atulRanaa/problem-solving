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
#include <utility>
#include <type_traits>

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

template <typename T, typename = void>
struct is_iterable : std::false_type {};
template <typename T>
struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T&>())),
                                    decltype(std::end(std::declval<T&>()))>>
    : std::true_type {};

template <typename T> void pr(const T& x); // forward decl for recursion
template <typename A, typename B> void pr_one(const std::pair<A, B>& p) {
    std::cerr << "{"; pr(p.first); std::cerr << ","; pr(p.second); std::cerr << "}";
}
template <typename T> void pr_one(const T& c) {
    if constexpr (is_iterable<T>::value && !std::is_same_v<std::decay_t<T>, std::string>) {
        std::cerr << "[";
        bool f = true;
        for (const auto& x : c) { std::cerr << (f ? "" : ", "); pr(x); f = false; }
        std::cerr << "]";
    } else {
        std::cerr << c;
    }
}

template <typename T> void pr(const T& x) { pr_one(x); }
#define inspect(x) std::cerr << "[INSPECT] " << #x << " = ", pr(x), std::cerr << "\n"


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

    string a,b;

    cin >> a;
    cin >> b;

    int n = a.size(), m = b.size();
    vector< vector<ll> > dp(n + 5, vector<ll> (m + 5, inf));

    for (int i = 0; i <= n; i++) dp[i][0] = i;
    for (int j = 0; j <= m; j++) dp[0][j] = j;

    for(int i = 1; i <= n; i++) {
        for(int j = 1; j <= m; j++) {
            if(a[i-1] == b[j-1])
                dp[i][j] = dp[i-1][j-1];
            else
                dp[i][j] = min({dp[i-1][j], dp[i-1][j-1], dp[i][j-1]}) + 1;
        }
    }

    cout << dp[n][m] << "\n";

    return 0;
}