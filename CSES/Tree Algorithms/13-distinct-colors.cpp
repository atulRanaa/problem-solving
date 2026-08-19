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

struct Query {
    int r, l, index;
};

int main() {
    fast;

    ll n; cin >> n;

    vector<ll> arr(n+1);
    vector< vector<int> > adj(n + 1);
    for(int i = 1; i <= n; i++) {
        cin >> arr[i];
    }

    int a, b;
    for(int i = 2; i <= n; i++) {
        cin >> a >> b;

        adj[a].push_back(b);
        adj[b].push_back(a);
    }

    vector<int> in(n + 1), out(n + 1);
    vector<int> par(n + 1), depth(n + 1);
    int timer = 0;
    function<void(int, int)> dfs1 = [&](int u, int parent) {
        par[u] = parent;

        in[u] = ++timer;

        for(int v: adj[u]) {
            if (v == parent)
                continue;

            depth[v] = depth[u] + 1;
            dfs1(v, u);
        }
        out[u] = timer;
    };

    dfs1(1, 0);

    vector< tuple<int, int, int> > queries;
    vector<int> order(n + 1);
    for(int i = 1; i <= n; i++) {
        order[in[i]] = i;
        queries.push_back({out[i], in[i], i});
    }
    sort(all(queries));

    fenwick_tree<ll> tree(n + 5);
    vector<int> ans(n + 1);
    map<int, int> prev;
    int itr = 0;
    for(auto& [r, l, index]: queries) {
        while(itr < r) {
            itr++;
            int node = order[itr];
            int color = arr[node];

            auto it = prev.find(color);
            if(it != prev.end()) {
                tree.add(it->second, -1);
            }

            tree.add(itr, 1);
            prev[color] = itr;
        }

        ans[index] = tree.sum(l, r + 1);
    }

    // dbg(in);
    // dbg(out);
    // dbg(order);
    // dbg(ans);

    for(int i = 1; i <= n; i++) {
        cout << ans[i] << " ";
    }
    cout << "\n";

    return 0;
}