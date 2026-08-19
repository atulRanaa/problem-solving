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

template <typename T, bool MINIMIZE = true>
struct LiChaoStatic {
private:
    struct Line {
        T m, b;
        T eval(T x) const { return m * x + b; }
    };

    struct Node {
        Line line;
        int  left, right;   // children indices, 0 = null
        bool has_line;
    };
    
    vector<Node> pool;
    int root;
    T L, R;

    // For MINIMIZE: +INF line. For MAXIMIZE: -INF line.
    static constexpr T WORST = MINIMIZE ? (T)2e18 : (T)(-2e18);

    static bool better(T a, T b) {
        return MINIMIZE ? a < b : a > b;
    }

    int new_node() {
        pool.push_back({{0, WORST}, 0, 0, false});
        return pool.size() - 1;
    }

public:
    explicit LiChaoStatic(T L, T R) : L(L), R(R) {
        pool.push_back({{0, WORST}, 0, 0, false});  // pool[0] = null sentinel
        root = new_node();
    }

    // add line y = m*x + b to entire range
    void add_line(T m, T b) {
        add_line_impl(root, L, R, {m, b});
    }

    // add line segment y = m*x + b for x in [xl, xr]
    void add_segment(T m, T b, T xl, T xr) {
        add_segment_impl(root, L, R, {m, b}, xl, xr);
    }

    void add_line_impl(int node, T l, T r, Line line) {
        if (!pool[node].has_line) {
            pool[node].line     = line;
            pool[node].has_line = true;
            return;
        }
        T mid = l + (r - l) / 2;
        bool left_better  = better(line.eval(l),   pool[node].line.eval(l));
        bool mid_better   = better(line.eval(mid),  pool[node].line.eval(mid));

        if (mid_better) swap(pool[node].line, line);   // new line dominates at mid

        if (l == r) return;

        if (left_better != mid_better) {
            if (!pool[node].left) pool[node].left = new_node();
            add_line_impl(pool[node].left, l, mid, line);
        } else {
            if (!pool[node].right) pool[node].right = new_node();
            add_line_impl(pool[node].right, mid + 1, r, line);
        }
    }

    void add_segment_impl(int node, T l, T r, Line line, T xl, T xr) {
        if (xr < l || r < xl) return;         // no overlap
        if (xl <= l && r <= xr) {             // full overlap
            add_line_impl(node, l, r, line);
            return;
        }
        T mid = l + (r - l) / 2;
        if (!pool[node].left)  pool[node].left  = new_node();
        if (!pool[node].right) pool[node].right  = new_node();
        add_segment_impl(pool[node].left,  l,     mid, line, xl, xr);
        add_segment_impl(pool[node].right, mid+1, r,   line, xl, xr);
    }

    T query(T x) const {
        return query_impl(root, L, R, x);
    }

    T query_impl(int node, T l, T r, T x) const {
        if (!node) return WORST;
        T res = pool[node].has_line ? pool[node].line.eval(x) : WORST;
        if (l == r) return res;
        T mid = l + (r - l) / 2;
        T child_res;
        if (x <= mid)
            child_res = query_impl(pool[node].left,  l,     mid, x);
        else
            child_res = query_impl(pool[node].right, mid+1, r,   x);
        return better(child_res, res) ? child_res : res;
    }
};


solve(int l, int t, ) {

}

int main() {
    fast;    
    ll n, k; cin >> n >> k;

    vector<ll> arr(n), prefix(n + 1, 0);
    
    for(int i = 1; i <= n; i++) {
        cin >> arr[i];
        prefix[i] = prefix[i-1] + arr[i];
    }

    vector< vector<ll> > dp(k + 1, vector<ll> (n + 1, inf));
    dp[0][0] = 0;

    // dp[j][i] divide i length array into j subarrays
    // for(int g = 1; g <= k; g++){
    //     for(int i = 1; i <= n; i++) {
    //         for(int j = 0; j < i; j++) {
    //             dp[g][i] = min(dp[g][i], dp[g-1][j] + ((prefix[i] - prefix[j]) * (prefix[i] - prefix[j])));
    //         }
    //     }
    // }

    // dp[g-1][j] + Pi^2 - 2*Pi Pj + Pj^2
    // for given i => Pi^2 -2*Pj (Pi) + (Pj^2 + dp[g-1][j])

    for(int i = 1; i <= n; i++) {
        ll result = tre
    }

    
    return 0;
} 