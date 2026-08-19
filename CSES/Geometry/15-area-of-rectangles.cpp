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



struct segtree {
    vector<ll> count, active_len;

    segtree(int n) {
        count.resize(4*n, 0);
        active_len.resize(4*n, 0);
    }

    void pull(int node, int l, int r) {
        if(count[node] > 0) 
            active_len[node] = r - l + 1;
        else {
            active_len[node] = ((l == r)?0:(active_len[2*node] + active_len[2*node + 1]));
        }
    }

    void update(int node, int l, int r, int L, int R, int val) {
        if(R < l || r < L) return;
        if(L <= l && r <= R) {
            count[node] += val;
            pull(node, l, r);
            return;
        }

        int mid = (l + r) >> 1;
        update(2*node, l, mid, L, R, val);
        update(2*node + 1, mid + 1, r, L, R, val);

        pull(node, l, r);
    }
};

int main() {
    fast;

    const int N = 2e6 + 10;
    const int offset = 1e6 + 5;
    segtree tree(N);


    int n; cin >> n;

    vector< tuple<int, int, int, int> > events;
    for(int i = 0; i < n; i++) {
        int a, b, c, d;
        cin >> a >> b >> c >> d;

        b += offset;
        d += offset;

        events.emplace_back(min(a, c), 1, min(b, d), max(b, d) - 1);
        events.emplace_back(max(a, c), -1, min(b, d), max(b, d) - 1);
    }
    
    sort(all(events));

    ll ans = 0;
    int last_x = get<0>(events[0]);

    for(int i = 0; i < (int)events.size(); i++) {
        auto [x, val, y1, y2] = events[i];

        ans += (x - last_x) * tree.active_len[1];
        tree.update(1, 0, N - 1, y1, y2, val);

        last_x = x;
    }

    cout << ans << "\n";
    return 0;
}