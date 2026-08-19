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

template <class S, S (*op)(S, S), S (*e)()>
struct persistent_segtree {
private:

    struct Node {
        S   val;
        int left, right;   // child indices; 0 = null
    };

    static constexpr int MAXNODES = 20000000;
    inline static Node pool[MAXNODES];
    inline static int  pool_ptr = 1;   // 0 reserved as null/identity node

    int         _n;
    vector<int> roots;     // roots[i] = root node index of version i

    int new_node(S val, int lc = 0, int rc = 0) {
        assert(pool_ptr < MAXNODES);
        pool[pool_ptr] = {val, lc, rc};
        return pool_ptr++;
    }

    // Build a full tree from array [l, r] — O(n)
    int build(const vector<S>& v, int l, int r) {
        if (l == r)
            return new_node(l < (int)v.size() ? v[l] : e());
        int mid = l + (r - l) / 2;
        int lc  = build(v, l,     mid);
        int rc  = build(v, mid+1, r  );
        return new_node(op(pool[lc].val, pool[rc].val), lc, rc);
    }

    // Path-copying point SET — creates O(log n) new nodes
    int update_set(int prev, int l, int r, int idx, S val) {
        if (l == r) return new_node(val);
        int mid = l + (r - l) / 2;
        int lc  = pool[prev].left;
        int rc  = pool[prev].right;
        if (idx <= mid) lc = update_set(lc, l,     mid, idx, val);
        else            rc = update_set(rc, mid+1, r,   idx, val);
        return new_node(op(pool[lc].val, pool[rc].val), lc, rc);
    }

    // Path-copying point ADD (op applied on top) — for frequency trees
    int update_add(int prev, int l, int r, int idx, S diff) {
        if (l == r) return new_node(op(pool[prev].val, diff));
        int mid = l + (r - l) / 2;
        int lc  = pool[prev].left;
        int rc  = pool[prev].right;
        if (idx <= mid) lc = update_add(lc, l,     mid, idx, diff);
        else            rc = update_add(rc, mid+1, r,   idx, diff);
        return new_node(op(pool[lc].val, pool[rc].val), lc, rc);
    }

    // Range query on a single version
    S query(int node, int l, int r, int ql, int qr) {
        if (!node || ql > r || qr < l) return e();
        if (ql <= l && r <= qr) return pool[node].val;
        int mid = l + (r - l) / 2;
        return op(query(pool[node].left,  l,     mid, ql, qr),
                  query(pool[node].right, mid+1, r,   ql, qr));
    }

    // Kth order statistic via two-version subtraction
    // Precondition: S is numeric, op = addition, e() = 0
    // ln/rn are root indices of two versions; k is 1-indexed
    int kth_impl(int ln, int rn, int l, int r, ll k) {
        if (l == r) return l;
        int mid    = l + (r - l) / 2;
        ll left_cnt = (ll)pool[pool[rn].left].val
                    - (ll)pool[pool[ln].left].val;
        if (k <= left_cnt) {
            return kth_impl(pool[ln].left,  pool[rn].left,  l,     mid, k);
        } else {
            return kth_impl(pool[ln].right, pool[rn].right, mid+1, r,   k - left_cnt);
        }
    }

public:

    // Empty tree of size n (all identity)
    explicit persistent_segtree(int n) : _n(n) {
        vector<S> v(n, e());
        roots.push_back(build(v, 0, n - 1));
    }

    // Build from existing array (version 0)
    explicit persistent_segtree(const vector<S>& v) : _n((int)v.size()) {
        roots.push_back(build(v, 0, _n - 1));
    }

    // Point set: version[ver] with position idx changed to val
    int set(int ver, int idx, S val) {
        assert(0 <= ver && ver < (int)roots.size());
        assert(0 <= idx && idx < _n);
        roots.push_back(update_set(roots[ver], 0, _n - 1, idx, val));
        return (int)roots.size() - 1;
    }

    void inplace_set(int ver, int idx, S val) {
        assert(0 <= ver && ver < (int)roots.size());
        assert(0 <= idx && idx < _n);
        roots[ver] = update_set(roots[ver], 0, _n - 1, idx, val);
    }

    // Point add: version[ver] with op(arr[idx], diff) at idx
    // Primary use: frequency trees (diff = 1 to insert element)
    int add(int ver, int idx, S diff) {
        assert(0 <= ver && ver < (int)roots.size());
        assert(0 <= idx && idx < _n);
        roots.push_back(update_add(roots[ver], 0, _n - 1, idx, diff));
        return (int)roots.size() - 1;
    }

    // Single point get on version ver (0-indexed)
    S get(int ver, int idx) {
        assert(0 <= ver && ver < (int)roots.size());
        return query(roots[ver], 0, _n - 1, idx, idx);
    }

    // Range query [l, r] on version ver (0-indexed, inclusive)
    S prod(int ver, int l, int r) {
        assert(0 <= ver && ver < (int)roots.size());
        assert(0 <= l && l <= r && r < _n);
        return query(roots[ver], 0, _n - 1, l, r);
    }

    // All elements query on version ver
    S all_prod(int ver) {
        assert(0 <= ver && ver < (int)roots.size());
        return pool[roots[ver]].val;
    }

    // Kth smallest among elements inserted between version ver_l and ver_r
    // ver_l is exclusive lower bound (typically ver_l = build ver = 0)
    // ver_r is inclusive upper bound
    // k is 1-indexed
    // Returns: coordinate index into the seg tree range [0, n)
    //          → decompress with your sorted array externally
    // PRECONDITION: S = int/ll, op = add, e() = 0
    int kth(int ver_l, int ver_r, ll k) {
        assert(0 <= ver_l && ver_r < (int)roots.size());
        return kth_impl(roots[ver_l], roots[ver_r], 0, _n - 1, k);
    }

    int latest()       const { return (int)roots.size() - 1; }
    int num_versions() const { return (int)roots.size(); }

    // Reset shared pool between test cases
    static void reset_pool() {
        pool_ptr = 1;
        pool[0]  = {e(), 0, 0};   // ensure null node holds identity
    }

    int copy_root(int k) {
        roots.push_back(roots[k]);

        return (int)roots.size() - 1;
    }
};

ll op(ll a, ll b) { return a + b;}
ll e() { return 0;}

using p_segtree = persistent_segtree<ll, op, e>;

int main() {
    fast;

    ll n, q; cin >> n >> q;
    ll query_t, k, a, b, x;
    
    vector<ll> arr(n+1);
    for(int i = 1; i <= n; i++) {
        cin >> x;
        arr[i] = x;
    }

    p_segtree tree(arr);

    while(q-- > 0) {
        cin >> query_t;
        if(query_t == 1) {
            cin >> k >> a >> x;
            --k;
            
            tree.inplace_set(k, a, x);
        } else if (query_t == 2) {
            cin >> k >> a >> b;
            --k;

            cout << tree.prod(k, a, b) << "\n";
        } else {
            cin >> k;
            --k;

            tree.copy_root(k);
        }
    }
    return 0;
}