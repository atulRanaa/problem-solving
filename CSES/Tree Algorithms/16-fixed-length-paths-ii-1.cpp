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
 
struct Decomposition {
private:
    int n, k1, k2;
    vector< vector<int> > adj;
 
    vector<int> sz;
    vector<bool> is_removed;
    int root, max_depth, max_curr_depth;
    ll k_path;
 
    vector<int> cnt;
    vector<ll> total_cnt;
 
    void dfs_size(int u, int parent) {
        sz[u] = 1;
        for(int v: adj[u]) {
            if (v == parent || is_removed[v]) continue;
            dfs_size(v, u);
            sz[u] += sz[v];
        }
    }
 
    int find_centroid(int u, int parent, int comp_size) {
        for(int v: adj[u]) {
            if (v == parent || is_removed[v])
                continue;
 
            if(sz[v] > comp_size/2) {
                return find_centroid(v, u, comp_size);
            }
        }
 
        return u;
    };
 
    void subtree_depths(int u, int parent, int dist) {
        if(dist > k2) return;
        cnt[dist]++;
        max_curr_depth = max(max_curr_depth, dist);
 
        for(int v: adj[u]) {
            if(v == parent || is_removed[v]) continue;
            subtree_depths(v, u, dist + 1);
        }
    }

    void process_centeroid(int centroid) {
        max_depth = 0;
        total_cnt[0] = 1;
        ll partial_sum_init = (k1 == 1 ? 1LL : 0LL);
 
        for(int v: adj[centroid]) {
            if(is_removed[v]) continue;
            
            max_curr_depth = 0;
            subtree_depths(v, centroid, 1);
            

            ll partial_sum = partial_sum_init;
            for(int d = 1; d <= max_curr_depth; d++) {
                k_path += partial_sum * cnt[d];

                if(k1 - d - 1 >= 0) partial_sum += total_cnt[k1 - d - 1];
                if(k2 - d >= 0) partial_sum -= total_cnt[k2 - d];
            }

            for(int d = max(k1 - 1, 0); d <= k2 - 1 && d <= max_curr_depth; d++)
                partial_sum_init += cnt[d];
            for(int d = 1; d <= max_curr_depth; d++) {
                total_cnt[d] += cnt[d];
            }
            
            max_depth = max(max_depth, max_curr_depth);
            fill(cnt.begin(), cnt.begin() + max_curr_depth + 1, 0);
        }
        fill(total_cnt.begin(), total_cnt.begin() + max_depth + 1, 0);
    }
 
    int decompose(int u, int parent, int cd_depth) {
        dfs_size(u, -1);
        int centroid = find_centroid(u, -1, sz[u]);
        
        process_centeroid(centroid);

        is_removed[centroid] = true;
        for(int v: adj[centroid]) {
            if(is_removed[v]) continue;
            decompose(v, centroid, cd_depth + 1);
        }
 
        return centroid;
    };
public:
    Decomposition(int n, int k1, int k2): 
        n(n), k1(k1), k2(k2), 
        adj(n + 1),
        sz(n + 1), is_removed(n + 1, false),
        k_path(0),
        cnt(k2 + 15, 0),
        total_cnt(k2 + 15, 0) {}
    
    void add_edge(int a, int b) {
        adj[a].push_back(b);
        adj[b].push_back(a);
    }
    void build() {
        root = decompose(1, -1, 0);
    }
 
    ll k_length_paths() {
        return k_path;
    }
};
 
int main() {
    fast;
 
    ll n, k1, k2; cin >> n >> k1 >> k2;
 
    Decomposition cd(n, k1, k2); // inclusive
    int a, b;
    for(int i = 2; i <= n; i++) {
        cin >> a >> b;
        cd.add_edge(a, b);
    }
 
    cd.build();
    cout << cd.k_length_paths() << "\n";
 
    return 0;
}