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
    return i >= 1 && j >= 1 && i <= n && j <= m;
}

struct Dinic {
private:
    struct Edge {
        int to, rev;
        ll  cap;
    };

    int n;
    vector<vector<Edge>> graph;
    vector<int> level, iter;

    // BFS to build level graph from source s
    bool bfs(int s, int t) {
        fill(level.begin(), level.end(), -1);
        queue<int> Q;
        level[s] = 0;
        Q.push(s);
        while (!Q.empty()) {
            int v = Q.front(); 
            Q.pop();
            
            for (auto& e: graph[v]) {
                if (e.cap > 0 && level[e.to] < 0) {
                    level[e.to] = level[v] + 1;
                    Q.push(e.to);
                }
            }
        }
        return level[t] >= 0;
    }

    // DFS to push flow along level graph
    ll dfs(int v, int t, ll f) {
        if (v == t) return f;
        for (int& i = iter[v]; i < (int)graph[v].size(); i++) {
            Edge& e = graph[v][i];
            if (e.cap > 0 && level[v] < level[e.to]) {
                ll d = dfs(e.to, t, min(f, e.cap));
                if (d > 0) {
                    e.cap -= d;
                    graph[e.to][e.rev].cap += d;
                    return d;
                }
            }
        }
        return 0;
    }

public:
    explicit Dinic(int n) : n(n), graph(n), level(n), iter(n) {}

    // add directed edge u→v with capacity cap
    // automatically adds reverse edge with cap 0
    void add_edge(int u, int v, ll cap) {
        graph[u].push_back({v, (int)graph[v].size(),     cap});
        graph[v].push_back({u, (int)graph[u].size() - 1, 0  });
    }

    // add undirected edge (both directions with full cap)
    void add_edge_undirected(int u, int v, ll cap) {
        graph[u].push_back({v, (int)graph[v].size(),     cap});
        graph[v].push_back({u, (int)graph[u].size() - 1, cap});
    }

    // Returns maximum flow from s to t
    ll max_flow(int s, int t) {
        ll flow = 0;
        while (bfs(s, t)) {
            fill(iter.begin(), iter.end(), 0);
            ll d;
            while ((d = dfs(s, t, inf)) > 0) flow += d;
        }
        return flow;
    }

    // Min cut: call AFTER max_flow()
    // Returns {cut value, set of nodes reachable from s in residual}
    pair<ll, vector<bool>> min_cut(int s, int t) {
        ll flow = max_flow(s, t);
        vector<bool> visited(n, false);
        queue<int> Q;
        Q.push(s); 
        visited[s] = true;
        while (!Q.empty()) {
            int v = Q.front(); 
            Q.pop();
            
            for (auto& e : graph[v])
                if (e.cap > 0 && !visited[e.to]) {
                    visited[e.to] = true;
                    Q.push(e.to);
                }
        }
        return {flow, visited};
        // cut edges: u→v where visited[u]=true, visited[v]=false
    }

    // Recover flow on each original edge
    // Call after max_flow(); pass original capacity to compare
    vector<pair<int,ll>> get_flow_on_edges(vector<tuple<int,int,ll>>& original_edges) {
        vector<pair<int,ll>> result;
        int idx = 0;
        for (auto& [u, v, cap] : original_edges) {
            ll residual = graph[u][idx].cap;
            result.push_back({idx, cap - residual});
            idx++;
        }
        return result;
    }
};

int main() {
    fast;    
    int n, m;
    cin >> n >> m;

    Dinic mf(n);
    ll a, b;

    vector< pair<int, int> > edges;
    loop (i, m) {
        cin >> a >> b;

        --a;
        --b;
        edges.push_back({a, b});
        mf.add_edge_undirected(a, b, 1);
    }

    auto [cut, visited] = mf.min_cut(0, n-1);

    vector< pair<int, int> > ans;
    for(auto [u, v]: edges) {
        if(visited[u] ^ visited[v] ) {
            ans.push_back({u, v});
        }
    }

    cout << ans.size() << "\n";
    for(auto [u, v]: ans) {
        cout << u+1 << " " << v+1 << "\n";
    }
    return 0;
} 