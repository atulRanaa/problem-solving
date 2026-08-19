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

struct eulerian_undirected {
public:
    struct Edge {
        int to;
        int id;
    };

    int n;
    int edge_count;
    std::vector<std::vector<Edge>> adj;
    std::vector<int> deg;

    eulerian_undirected(int n) : n(n), edge_count(0), adj(n), deg(n, 0) {}

    // Add an undirected edge between u and v (0-indexed)
    void add_edge(int u, int v) {
        int id = edge_count++;
        adj[u].push_back({v, id});
        adj[v].push_back({u, id});
        deg[u]++;
        deg[v]++;
    }

    std::vector<int> get_eulerian_path() {
        int odd_count = 0;
        int start_node = 1;

        for (int i = 0; i < n; i++) {
            if (deg[i] & 1) return {};
            if (deg[i] & 1) {
                odd_count++;
                if (start_node == -1) {
                    start_node = i; // Path must start at an odd degree vertex
                }
            }
        }

        // Valid conditions: exactly 0 odd vertices (Circuit) or exactly 2 (Path)
        if (odd_count != 0 && odd_count != 2) {
            return {};
        }

        if (start_node == -1) {
            for (int i = 0; i < n; i++) {
                if (deg[i] > 0) {
                    start_node = i;
                    break;
                }
            }
            if (start_node == -1) start_node = 0; // Empty graph safe fallback
        }

        start_node = 1;

        // Hierholzer Algorithm
        std::vector<bool> used_edge(edge_count, false);
        std::vector<int> edge_idx(n, 0);
        std::vector<int> curr_path;
        std::vector<int> circuit;

        curr_path.push_back(start_node);

        while (!curr_path.empty()) {
            int u = curr_path.back();

            // Find the next available unvisited edge out of vertex 'u'
            while (edge_idx[u] < (int)adj[u].size() && used_edge[adj[u][edge_idx[u]].id]) {
                edge_idx[u]++; // Skip globally burned edges
            }

            if (edge_idx[u] < (int)adj[u].size()) {
                Edge e = adj[u][edge_idx[u]++]; // Burn edge on forward path
                used_edge[e.id] = true;         // Marks both directions as visited instantly
                curr_path.push_back(e.to);
            } else {
                // Dead end reached: freeze to path and backtrack
                circuit.push_back(u);
                curr_path.pop_back();
            }
        }

        // Reverse to get correct chronological path order
        std::reverse(circuit.begin(), circuit.end());

        // Ensures that all edges present in the graph were processed
        if ((int)circuit.size() != edge_count + 1) {
            return {}; // Graph has disconnected components containing separate edges
        }

        return circuit;
    }
};


int main() {
    fast;
    ll n, m; cin >> n >> m;
    ll a, b;

    eulerian_undirected G(n + 1);
    loop (i, m) {
        cin >> a >> b;

        G.add_edge(a, b);
    }

    vector<int> path = G.get_eulerian_path();
    if(path.empty())
        cout << "IMPOSSIBLE\n";
    else {
        for(int e: path) {
            cout << e << " ";
        }
        cout << "\n";
    }
    

    return 0;
}