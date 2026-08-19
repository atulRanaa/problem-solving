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
#include <bitset>

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

const int N = 5e4 + 5;
vector< bitset<N> > dp;


struct scc_graph {
  public:
    int n;
    int num_scc;
    std::vector<std::vector<int>> adj;
    std::vector<std::vector<int>> radj;
    std::vector<int> comp;

    scc_graph() : n(0), num_scc(0) {}
    scc_graph(int n) : n(n), num_scc(0), adj(n), radj(n), comp(n, -1) {}

    void add_edge(int u, int v) {
        adj[u].push_back(v);
        radj[v].push_back(u);
    }

    void build() {
        std::vector<bool> vis(n, false);
        std::vector<int> order;
        order.reserve(n);

        for (int i = 0; i < n; i++) {
            if (!vis[i]) {
                dfs1(i, vis, order);
            }
        }

        std::fill(comp.begin(), comp.end(), -1);
        num_scc = 0;

        for (int i = n - 1; i >= 0; i--) {
            int root = order[i];
            if (comp[root] == -1) {
                dfs2(root, num_scc++);
            }
        }
    }

    std::vector<std::vector<int>> condensation() {
        std::vector<std::vector<int>> dag(num_scc);
        std::vector<int> last_seen(num_scc, -1);

        for (int u = 0; u < n; u++) {
            int u_comp = comp[u];
            for (int v : adj[u]) {
                int v_comp = comp[v];
                if (u_comp != v_comp && last_seen[v_comp] != u_comp) {
                    dag[u_comp].push_back(v_comp);
                    last_seen[v_comp] = u_comp;
                }
            }
        }
        return dag;
    }

    std::vector<std::vector<int>> groups() {
        std::vector<int> group_sizes(num_scc, 0);
        for (int i = 0; i < n; i++) {
            group_sizes[comp[i]]++;
        }
        std::vector<std::vector<int>> res(num_scc);
        for (int i = 0; i < num_scc; i++) {
            res[i].reserve(group_sizes[i]);
        }
        for (int i = 0; i < n; i++) {
            res[comp[i]].push_back(i);
        }
        return res;
    }

  private:
    void dfs1(int node, std::vector<bool>& vis, std::vector<int>& order) {
        vis[node] = true;
        for (int v : adj[node]) {
            if (!vis[v]) dfs1(v, vis, order);
        }
        order.push_back(node);
    }

    void dfs2(int node, int id) {
        comp[node] = id;
        for (int v : radj[node]) {
            if (comp[v] == -1) dfs2(v, id);
        }
    }
};

void solve(int u, vector<bool> &visited, vector< vector<int> > &adj) {
    if(visited[u])
        return;
    visited[u] = true;

    for(int v: adj[u]) {
        solve(v, visited, adj);
        dp[u] |= dp[v];
    }

    dp[u][u] = 1;
}

int main() {
    fast;
    dp.resize(N);

    ll n, m, q;
    cin >> n >> m >> q;

    scc_graph ssc(n + 1);
    int a, b;
    for(int i = 0; i < m; i++) {
        cin >> a >> b;

        ssc.add_edge(a, b);
    }

    ssc.build();
    {
        auto adj = ssc.condensation();
        auto groups = ssc.groups();
        
        vector<bool> visited(ssc.num_scc, false);
        for(int i = 0; i < ssc.num_scc; i++) {
            if(!visited[i])
                solve(i, visited, adj);
        }
    }
    

    while(q-- > 0) {
        cin >> a >> b;

        int a_comp = ssc.comp[a];
        int b_comp = ssc.comp[b];

        // dbg(a, b, a_comp, b_comp);
        int is_connected = (a_comp == b_comp) || dp[a_comp][b_comp];
        cout << (is_connected? "YES": "NO") << "\n";
    }
    return 0;
}
