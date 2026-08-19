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
 
template <class T>
struct fenwick_tree {
    using U = typename std::make_unsigned<T>::type;
 
  public:
    fenwick_tree() : _n(0) {}
    explicit fenwick_tree(int n) : _n(n), data(n) {}
 
    void add(int p, T x) {
        assert(0 <= p && p < _n);
        p++;
        while (p <= _n) {
            data[p - 1] += U(x);
            p += p & -p;
        }
    }
 
    // sum of [0, r)
    T sum(int r) {
        assert(0 <= r && r <= _n);
        U s = 0;
        while (r > 0) {
            s += data[r - 1];
            r -= r & -r;
        }
        return T(s);
    }
 
    // sum of [l, r)
    T sum(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        return sum(r) - sum(l);
    }
 
    int kth(T k) {
        assert(_n > 0 && 1 <= k && k <= sum(_n));
        int pos = 0;
        // Start from highest power of 2 <= _n
        for (int pw = 1 << (31 - __builtin_clz(_n)); pw > 0; pw >>= 1) {
            if (pos + pw <= _n && data[pos + pw - 1] < U(k)) {
                pos += pw;
                k -= T(data[pos - 1]);
            }
        }
        return pos;  // 0-indexed result
    }
    
  private:
    int _n;
    std::vector<U> data;
};
 
template <class S, S (*op)(S, S), S (*e)()>
struct segtree {
  public:
    segtree() : segtree(0) {}
    segtree(int n) : segtree(std::vector<S>(n, e())) {}
    segtree(const std::vector<S>& v) : _n(int(v.size())) {
        log = 0;
        while ((1 << log) < _n) log++;
        size = 1 << log;
        d = std::vector<S>(2 * size, e());
        for (int i = 0; i < _n; i++) d[size + i] = v[i];
        for (int i = size - 1; i >= 1; i--) update(i);
    }
 
    void set(int p, S x) {
        assert(0 <= p && p < _n);
        p += size;
        d[p] = x;
        for (int i = 1; i <= log; i++) update(p >> i);
    }
 
    S get(int p) {
        assert(0 <= p && p < _n);
        return d[p + size];
    }
 
    S prod(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        S sml = e(), smr = e();
        l += size;
        r += size;
        while (l < r) {
            if (l & 1) sml = op(sml, d[l++]);
            if (r & 1) smr = op(d[--r], smr);
            l >>= 1;
            r >>= 1;
        }
        return op(sml, smr);
    }
 
    S all_prod() { return d[1]; }
 
    template <class F> int max_right(int l, F f) {
        assert(0 <= l && l <= _n);
        assert(f(e()));
        if (l == _n) return _n;
        l += size;
        S sm = e();
        do {
            while (l % 2 == 0) l >>= 1;
            if (!f(op(sm, d[l]))) {
                while (l < size) {
                    l = 2 * l;
                    if (f(op(sm, d[l]))) { sm = op(sm, d[l]); l++; }
                }
                return l - size;
            }
            sm = op(sm, d[l]);
            l++;
        } while ((l & -l) != l);
        return _n;
    }
 
    template <class F> int min_left(int r, F f) {
        assert(0 <= r && r <= _n);
        assert(f(e()));
        if (r == 0) return 0;
        r += size;
        S sm = e();
        do {
            r--;
            while (r > 1 && (r % 2)) r >>= 1;
            if (!f(op(d[r], sm))) {
                while (r < size) {
                    r = 2 * r + 1;
                    if (f(op(d[r], sm))) { sm = op(d[r], sm); r--; }
                }
                return r + 1 - size;
            }
            sm = op(d[r], sm);
        } while ((r & -r) != r);
        return 0;
    }
 
  private:
    int _n, size, log;
    std::vector<S> d;
    void update(int k) { d[k] = op(d[2 * k], d[2 * k + 1]); }
};
 
// Range Sum
ll op_sum(ll a, ll b) { return a + b; }
ll e_sum() { return 0; }
using segtree_sum = segtree<ll, op_sum, e_sum>;
 
// Range Min
ll op_min(ll a, ll b) { return min(a, b); }
ll e_min() { return inf; }   // use a large sentinel, e.g. const ll inf = 1e18;
using segtree_min = segtree<ll, op_min, e_min>;
 
// Range Max
ll op_max(ll a, ll b) { return max(a, b); }
ll e_max() { return -inf; }
using segtree_max = segtree<ll, op_max, e_max>;
 
 
int main() {
    fast;
 
    ll n, q; cin >> n >> q;
    vector<ll> arr(n + 1);
 
    for(int i = 1; i <= n; i++) {
        cin >> arr[i];
    }
 
    segtree_min tree(arr);
    
    
    while(q-- > 0) {
        int t, a, b;
        cin >> t >> a >> b;
        if(t == 1) {
            tree.set(a, b);
            arr[a] = b;
        } else {
            cout << tree.prod(a, b + 1) << "\n";
        }
    }
 
    return 0;
}