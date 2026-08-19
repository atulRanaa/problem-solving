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

bool within(int i, int j, int n, int m) {
    return i >= 0 && j >= 0 && i < n && j < m;
}

template <class T>
struct fenwick_tree2d {
    using U = typename std::make_unsigned<T>::type;

  public:
    fenwick_tree2d() : _n(0), _m(0) {}
    explicit fenwick_tree2d(int n, int m) : _n(n), _m(m), data(n, std::vector<U>(m, 0)) {}

    void add(int r, int c, T x) {
        assert(0 <= r && r < _n);
        assert(0 <= c && c < _m);
        for (int i = r + 1; i <= _n; i += i & -i) {
            for (int j = c + 1; j <= _m; j += j & -j) {
                data[i - 1][j - 1] += U(x);
            }
        }
    }

    // exclusive bounds [0, r) x [0, c)
    T sum(int r, int c) {
        assert(0 <= r && r <= _n);
        assert(0 <= c && c <= _m);
        U s = 0;
        
        for (int i = r; i > 0; i -= i & -i) {
            for (int j = c; j > 0; j -= j & -j) {
                s += data[i - 1][j - 1];
            }
        }
        return T(s);
    }

    // sum over half-open ranges [r1, r2) x [c1, c2)
    T sum(int r1, int c1, int r2, int c2) {
        assert(0 <= r1 && r1 <= r2 && r2 <= _n);
        assert(0 <= c1 && c1 <= c2 && c2 <= _m);
        return sum(r2, c2) - sum(r1, c2) - sum(r2, c1) + sum(r1, c1);
    }
    
  private:
    int _n, _m;
    std::vector<std::vector<U>> data;
};
int main() {
    fast;

    ll n, q; cin >> n >> q;
    
    vector<string> grid(n);
    loop(i, n) {
        cin >> grid[i];
    }

    fenwick_tree2d<int> tree(n + 1, n + 1);

    for(int i = 1; i <= n; i++) {
        for(int j = 1; j <= n; j++) {
            if(grid[i-1][j-1] == '.')
                continue;

            tree.add(i, j, 1);
        }
    }

    int query_t, x1, y1, x2, y2;
    while(q-- > 0) {
        cin >> query_t;
        if(query_t == 1) {
            cin >> x1 >> y1;
            
            int value = (grid[x1-1][y1-1]=='*')?-1:1;
            tree.add(x1, y1, value);
            grid[x1-1][y1-1] = (grid[x1-1][y1-1]=='*')?'.':'*';
        } else {
            cin >> x1 >> y1 >> x2 >> y2;

            x2++;
            y2++;
            cout << tree.sum(x1, y1, x2, y2) << "\n";
        }
    }
    return 0;
}