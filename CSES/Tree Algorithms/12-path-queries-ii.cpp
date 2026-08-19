#include <climits>
#include <cmath>
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
#include <functional>

using namespace std;
typedef long long ll;

const ll MOD = 1e9 + 7;
const ll inf = 1e15;
#define all(x) x.begin(),x.end()
#define LCM(a,b) ((a*b)/__gcd(a,b))

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

struct HLD {
private:
    int n, timer;
    vector<vector<int>> adj;
    vector<int> parent, depth, sz, heavy, head, pos, pos_end;
    // pos[u]     = start position of u in flattened array
    // pos_end[u] = end position of subtree of u (exclusive)
    // order[i]   = which node maps to flattened index i

    // compute subtree sizes, identify heavy children
    void dfs_size(int root) {
        stack<pair<int,int>> stk;
        stk.push({root, 0});
        vector<int> order_stk;

        // Forward pass: set parent/depth, record traversal order
        while (!stk.empty()) {
            auto [u, p] = stk.top(); stk.pop();
            parent[u] = p;
            sz[u] = 1;
            order_stk.push_back(u);
            for (int v : adj[u]) {
                if (v == p) continue;
                depth[v] = depth[u] + 1;
                stk.push({v, u});
            }
        }

        // Reverse pass: compute sizes + heavy children bottom-up
        for (int i = (int)order_stk.size() - 1; i >= 0; i--) {
            int u = order_stk[i];
            int best = 0;
            for (int v : adj[u]) {
                if (v == parent[u]) continue;
                sz[u] += sz[v];
                if (sz[v] > best) { best = sz[v]; heavy[u] = v; }
            }
        }
    }

    // assign HLD positions
    void dfs_decompose(int root) {
        // Stack stores {node, chain_head}
        // Push light children with their own head, heavy child with same head
        stack<pair<int,int>> stk;
        stk.push({root, root});

        while (!stk.empty()) {
            auto [u, h] = stk.top(); stk.pop();
            head[u] = h;
            pos[u]  = timer;
            order[timer] = u;
            timer++;

            // Push light children first (they'll be processed last = after heavy chain)
            for (int v : adj[u]) {
                if (v == parent[u] || v == heavy[u]) continue;
                
                // light → new chain starting at v
                stk.push({v, v});
            }
            // Push heavy child last → it pops first → chain stays contiguous
            if (heavy[u] != -1) stk.push({heavy[u], h});
        }

        // Subtree of u = [pos[u], pos[u] + sz[u])
        for (int u = 1; u <= n; u++)
            pos_end[u] = pos[u] + sz[u];
    }

    // collect chain segments between u and v
    // Returns list of [l, r) half-open intervals in flattened array
    // For edge queries: exclude the LCA node (top node of last segment)
    vector<pair<int,int>> path_segments(int u, int v, bool edge_query = false) {
        vector<pair<int,int>> segs;
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) 
                swap(u, v);
            segs.push_back({pos[head[u]], pos[u] + 1});   // [pos[head[u]], pos[u])
            u = parent[head[u]];
        }
        if (depth[u] > depth[v]) swap(u, v);
        // For edge queries: exclude LCA itself (its edge is to parent, not in path)
        int lo = edge_query ? pos[u] + 1 : pos[u];
        segs.push_back({lo, pos[v] + 1});
        return segs;
    }

public:
    vector<int> order;

    // inclusive
    explicit HLD(int n)
        : n(n), timer(0),
          adj(n + 1), parent(n + 1, 0), depth(n + 1, 0),
          sz(n + 1, 0), heavy(n + 1, -1), head(n + 1, 0),
          pos(n + 1, 0), pos_end(n + 1, 0), order(n + 1, 0) {}

    void add_edge(int u, int v) { 
        adj[u].push_back(v); 
        adj[v].push_back(u); 
    }


    void init(int root = 1) {
        timer = 0;
        dfs_size(root);
        dfs_decompose(root);
    }

    int lca(int u, int v) {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            u = parent[head[u]];
        }
        return depth[u] < depth[v] ? u : v;
    }

    // Better: pass combiner and identity explicitly
    template <typename Tree, typename T, typename Combine>
    T query_path(int u, int v, Tree& seg, T identity, Combine combine) {
        T res = identity;
        for (auto [l, r] : path_segments(u, v))
            res = combine(res, seg.prod(l, r));
        return res;
    }

    template <typename Tree, typename T, typename Combine>
    T query_path_edge(int u, int v, Tree& seg, T identity, Combine combine) {
        T res = identity;
        for (auto [l, r] : path_segments(u, v, true))
            res = combine(res, seg.prod(l, r));
        return res;
    }

    // Path update (range apply)
    template <typename Tree, typename F>
    void update_path(int u, int v, Tree& seg, F f) {
        for (auto [l, r] : path_segments(u, v))
            seg.apply(l, r, f);
    }

    template <typename Tree, typename F>
    void update_path_edge(int u, int v, Tree& seg, F f) {
        for (auto [l, r] : path_segments(u, v, true))
            seg.apply(l, r, f);
    }

    // Subtree query — [pos[u], pos[u] + sz[u]) is the entire subtree
    template <typename Tree>
    auto query_subtree(int u, Tree& seg) {
        return seg.prod(pos[u], pos_end[u]);
    }

    // Subtree update
    template <typename Tree, typename F>
    void update_subtree(int u, Tree& seg, F f) {
        seg.apply(pos[u], pos_end[u], f);
    }

    // Point set
    template <typename Tree, typename S>
    void update_node(int u, Tree& seg, S val) {
        seg.set(pos[u], val);
    }
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

// Range Max
ll op_max(ll a, ll b) { return max(a, b); }
ll e_max() { return -inf; }
using segtree_max = segtree<ll, op_max, e_max>;

int main() {
    fast;

    ll n, q; cin >> n >> q;

    vector<ll> arr(n+1);    
    for(int i = 1; i <= n; i++) {
        cin >> arr[i];
    }

    HLD hld(n); // n inclusive
    int a, b;
    for(int i = 2; i <= n; i++) {
        cin >> a >> b;

        hld.add_edge(a, b);
    }
    hld.init(1);

    vector<ll> flat(n);
    for(int i = 0; i < n; i++) {
        flat[i] = arr[hld.order[i]];
    }
    segtree_max tree(flat);

    int query_t;
    while(q-- > 0) {
        cin >> query_t >> a >> b;


        if(query_t == 1) {
            hld.update_node(a, tree, b);
            arr[a] = b;
        } else {
            cout << hld.query_path(a, b, tree, e_max(), op_max) << " ";
        }
    }
    cout << "\n";

    return 0;
}