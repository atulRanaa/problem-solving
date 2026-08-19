#include <climits>
#include <functional>
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
#include <cmath>
#include <bitset>
#include <iomanip>
#include <complex>
 
using namespace std;
typedef long long ll;
typedef unsigned long long ull;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define LCM(a,b) ((a*b)/__gcd(a,b))
#define inf 2e9
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

ll cross_product(complex<ll> a, complex<ll> b) {
    auto result = conj(a) * b;
    return result.imag();
};

ll cross_product(pair<ll, ll> &a, pair<ll, ll> &b) {
    return a.first * b.second - b.first * a.second;
}

bool on_line(pair<ll, ll> a, pair<ll, ll> b, pair<ll, ll> c) {
    complex<ll> p1 = {a.first, a.second};
    complex<ll> p2 = {b.first, b.second};
    complex<ll> p3 = {c.first, c.second};
    auto result = cross_product(p2 - p1, p3 - p1);

    bool x_overlap = min(a.first, b.first) <= c.first && c.first <= max(a.first, b.first);
    bool y_overlap = min(a.second, b.second) <= c.second && c.second <= max(a.second, b.second);
    return result == 0 && (x_overlap && y_overlap);
}

bool line_intersection(pair<ll, ll> a, pair<ll, ll> b, pair<ll, ll> c, pair<ll, ll> d) {
    // check if line ab intersect with line cd
    auto [x1, y1] = a;
    auto [x2, y2] = b;
    auto [x3, y3] = c; 
    auto [x4, y4] = d;

    complex<ll> p1 = {x1, y1};
    complex<ll> p2 = {x2, y2};
    complex<ll> p3 = {x3, y3};
    complex<ll> p4 = {x4, y4};

    bool intersect = false;
    ll cp1 = cross_product(p2 - p1, p3 - p1);
    ll cp2 = cross_product(p2 - p1, p4 - p1);

    ll cp3 = cross_product(p4 - p3, p1 - p3);
    ll cp4 = cross_product(p4 - p3, p2 - p3);
    // line intersection
    
    if(cp1 == 0 && cp2 == 0 && cp3 == 0 && cp4 == 0) {
        bool x_overlap = max( min(x1,x2), min(x3,x4)) <= min(max(x1, x2), max(x3, x4));
        bool y_overlap = max( min(y1,y2), min(y3,y4)) <= min(max(y1, y2), max(y3, y4));

        if(x_overlap && y_overlap)
            intersect = true;
    } else {
        bool plane1 = (cp1 >= 0 && cp2 <= 0) || (cp1 <= 0 && cp2 >= 0);
        bool plane2 = (cp3 >= 0 && cp4 <= 0) || (cp3 <= 0 && cp4 >= 0);

        if(plane1 && plane2) {
            intersect = true;
        }
    }

    return intersect;
}

ll polygon_area(vector< pair<ll, ll> > &arr) {
    // shoelace formula return 2 * area

    // NOTE: last element should be same as first
    ll ans = 0;
    for(int i = 1; i < (int)arr.size(); i++) {
        ans += cross_product(arr[i-1], arr[i]);
    }

    return abs(ans);
}

ll points_onboundary(vector< pair<ll, ll> > &arr) {
    ll ans = 0;
    for(int i = 1; i < (int)arr.size(); i++) {
        ans += gcd(abs(arr[i-1].first - arr[i].first), abs(arr[i-1].second - arr[i].second));
    }

    return ans;
}

struct Point {
    ll x, y;

    bool operator<(const Point& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }
};

ll cross_product(Point &a, Point &b, Point &c) {
    // b-a, c-b
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}


vector<Point> convex_hull(vector<Point>& pts) {
    int n = pts.size();
    if (n <= 2) return pts;

    sort(pts.begin(), pts.end());
    vector<Point> hull;

    // lower hull
    for (int i = 0; i < n; i++) {
        // Use '< 0' to include collinear boundary points
        // Use '<= 0' if you ONLY want strict corner vertices
        while (hull.size() >= 2 && cross_product(hull[hull.size() - 2], hull.back(), pts[i]) < 0) {
            hull.pop_back();
        }
        hull.push_back(pts[i]);
    }

    // upper hull
    int lower_size = hull.size();
    for (int i = n - 2; i >= 0; i--) {
        while ((int)hull.size() > lower_size && cross_product(hull[hull.size() - 2], hull.back(), pts[i]) < 0) {
            hull.pop_back();
        }
        hull.push_back(pts[i]);
    }

    hull.pop_back();
    return hull;
}

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


template <typename T, bool MINIMIZE = true>
struct LiChaoStatic {
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

int main() {
    fast;
    
    const int N = 1e5 + 10; 
    LiChaoStatic<ll, false> tree(0, N);


    ll n, m; 
    cin >> n >> m;

    ll y1, y2;
    for(int i = 0; i < n; i++) {
        cin >> y1 >> y2;

        tree.add_line((y2 - y1) / m, y1);
    }

    for(int i = 0; i <= m; i++) {
        cout << tree.query(i) << " ";
    }
    cout << "\n";
    return 0;
}