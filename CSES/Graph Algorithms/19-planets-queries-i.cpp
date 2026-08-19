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
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cassert>
 
using namespace std;
typedef long long ll;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define LCM(a,b) ((a*b)/__gcd(a,b))
#define inf 1e18
#define test ll cse;cin>>cse;for(ll _i=1;_i<=cse;_i++)
#define PI 3.14159265
#define fast ios_base::sync_with_stdio(false);cin.tie(NULL);cout.tie(NULL);
#define loop(i, n) for(int i = 0; i < n; i++)
const double EPS = 1E-9;
template <typename T> using min_heap = priority_queue<T, vector<T>, greater<T>>;
 
 
template <typename T, typename = void> struct is_iter : false_type {};
template <typename T> struct is_iter<T, void_t<decltype(begin(declval<T&>()))>> : true_type {};
 
template <typename T>
constexpr bool is_str_v = is_same_v<decay_t<T>, string> || is_same_v<decay_t<T>, char> || is_same_v<decay_t<T>, const char*>;
 
template <typename T> int width_of(const T& x) { ostringstream o; o << x; return (int)o.str().size(); }
 
template <typename A, typename B> void pr(const pair<A, B>& p, int d = 0);
 
template <typename T> void pr(const T& x, int d = 0) {
    using D = decay_t<T>;
    if constexpr (is_str_v<D>) cerr << x;
    else if constexpr (is_iter<T>::value) {
        using V = decay_t<decltype(*begin(x))>;
        if constexpr (is_iter<V>::value && !is_str_v<V>) {       // matrix: container of containers
            int w = 0;
            for (auto& row : x) for (auto& e : row) w = max(w, width_of(e));
            for (auto& row : x) {
                cerr << string(d * 2, ' ');
                for (auto& e : row) cerr << setw(w) << e << ' ';
                cerr << "\n";
            }
        } else {                                                  // flat container
            for (auto& e : x) { pr(e, d); cerr << ' '; }
        }
    } else cerr << x;
}
 
template <typename A, typename B> void pr(const pair<A, B>& p, int d) {
    pr(p.first, d); cerr << ":"; pr(p.second, d);
}
 
template <typename T, typename... R> void pr_all(const T& f, const R&... r) {
    pr(f); if constexpr (sizeof...(r) > 0) { cerr << "  "; pr_all(r...); }
}
 
#ifdef LOCAL
#define dbg(...) cerr << #__VA_ARGS__ << ":\n", pr_all(__VA_ARGS__), cerr << "\n"
#else
#define dbg(...)
#endif
 
bool within(int i, int j, int n, int m) {
    return i >= 0 && j >= 0 && i < n && j < m;
}
 
ll binpow(ll a, ll b, ll m) {
    a %= m;
    if (a < 0) a += m;
    ll res = 1 % m;
    while (b > 0) {
        if (b & 1)
            res = res * a % m;
        a = a * a % m;
        b >>= 1;
    }
    return res;
}
 
// left, up, right, down
vector<int> dx = {0, -1, 0, 1};
vector<int> dy = {-1, 0, 1, 0};
string dir = "LURD";
 
 
int main() {
    fast;
    ll n, q; cin >> n >> q;
 
    vector<int> arr(n + 1);
    for(int i = 1; i <= n; i++) {
        cin >> arr[i];
    }
 
    vector< vector<int> > succ(n + 1, vector<int> (30, 0));
    
    for(int k = 0; k <= 29; k++) {
        for(int i = 1; i <= n; i++) {
            if(k == 0)
                succ[i][k] = arr[i];
            else 
                succ[i][k] = succ[ succ[i][k-1] ][k-1];
        }
    }
 
    // dbg(succ);
 
    int x, k;
    loop (i, q) {
        cin >> x >> k;
 
        for(int i = 0; i < 30; i++) {
            if(k & (1 << i))
                x = succ[x][i];
        }
        cout << x << "\n";
    }
    
    return 0;
}