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
#define inf 1e15
#define test ll cse;cin>>cse;for(ll _i=1;_i<=cse;_i++)
#define PI 3.14159265
#define fast ios_base::sync_with_stdio(false);cin.tie(NULL);cout.tie(NULL);
#define loop(i, n) for(int i = 0; i < n; i++)
const double EPS = 1E-9;

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

struct fenwick_tree_mod {
  public:
    fenwick_tree_mod() : _n(0), mod(1) {}
    explicit fenwick_tree_mod(int n, ll m) : _n(n), mod(m), data(n, 0) {}

    void add(int p, ll x) {
        assert(0 <= p && p < _n);
        x %= mod;
        if (x < 0) x += mod;

        p++;
        while (p <= _n) {
            data[p - 1] = (data[p - 1] + x) % mod;
            p += p & -p;
        }
    }

    // prefix sum of [0, r), mod m
    ll sum(int r) {
        assert(0 <= r && r <= _n);
        ll s = 0;
        while (r > 0) {
            s = (s + data[r - 1]) % mod;
            r -= r & -r;
        }
        return s;
    }

    // range sum of [l, r), mod m
    ll sum(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        ll res = (sum(r) - sum(l)) % mod;
        if (res < 0) res += mod;
        return res;
    }

  private:
    int _n;
    ll mod;
    vector<ll> data;
};


int main() {
    fast;

    ll n, q; cin >> n >> q;
    vector<ll> arr(n + 1);
    for(int i = 1; i <= n; i++) {
        cin >> arr[i];

        arr[i] += arr[i-1];
    }
    
    while(q-- > 0) {
        int a, b;
        cin >> a >> b;
        cout << arr[b] - arr[a-1] << "\n";
    }

    return 0;
}