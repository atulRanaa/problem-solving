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

struct Decomposition {
private:
    int n, K;
    vector< vector<int> > adj;

    vector<int> sz, is_removed, par_centeriod, depth;
    vector< vector<int> > center_adj;
    int root;
    ll k_path;

    vector<ll> seen;
    vector<int> used_indices;

    void dfs_size(int u, int parent) {
        sz[u] = 1;

        for(int v: adj[u]) {
            if (v == parent || is_removed[v])
                continue;

            dfs_size(v, u);
            sz[u] += sz[v];
        }
    };

    int find_centroid(int u, int parent, int comp_size) {
        for(int v: adj[u]) {
            if (v == parent || is_removed[v])
                continue;

            if(sz[v] > (comp_size/2)) {
                return find_centroid(v, u, comp_size);
            }
        }

        return u;
    };

    void subtree_depths(int u, int parent, int dist, vector<int> &curr_depths) {
        if(dist > K) return;
        curr_depths.push_back(dist);

        for(int v: adj[u]) {
            if(v == parent || is_removed[v]) continue;
            subtree_depths(v, u, dist + 1, curr_depths);
        }
    }

    int decompose(int u, int parent, int cd_depth) {
        dfs_size(u, 0);
        int centroid = find_centroid(u, 0, sz[u]);

        par_centeriod[centroid] = parent;
        depth[centroid] = cd_depth;
        if(parent != -1) {
            center_adj[parent].push_back(centroid);
        }

        {
            // k length path
            seen[0] = 1;
            used_indices.push_back(0);

            for(int v: adj[centroid]) {
                if(is_removed[v]) continue;
                
                vector<int> curr_depths;
                subtree_depths(v, centroid, 1, curr_depths);
                for(int d: curr_depths) {
                    k_path += seen[K - d];
                }

                for(int d: curr_depths) {
                    seen[d]++;
                    used_indices.push_back(d);
                }
            }

            for (int idx : used_indices) seen[idx] = 0;
            used_indices.clear();
        }
        
        
        is_removed[centroid] = true;
        for(int v: adj[centroid]) {
            if(is_removed[v]) continue;
            decompose(v, centroid, cd_depth + 1);
        }

        is_removed[centroid] = false;
        return centroid;
    };
public:
    Decomposition(int n, int k): 
        n(n), K(k), 
        adj(n + 1),
        sz(n + 1), is_removed(n + 1, 0), par_centeriod(n + 1), depth(n + 1),
        center_adj(n + 1), 
        k_path(0),
        seen(k + 1, 0) {}
    
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

    ll n, k; cin >> n >> k;

    Decomposition cd(n, k); // inclusive
    int a, b;
    for(int i = 2; i <= n; i++) {
        cin >> a >> b;
        cd.add_edge(a, b);
    }

    cd.build();
    cout << cd.k_length_paths() << "\n";

    return 0;
}