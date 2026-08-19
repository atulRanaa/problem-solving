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

int main() {
    fast;

    int n; cin >> n;
    vector< vector<int> > a(n, vector<int>(3));
    loop (i, n) {
        cin >> a[i][0] >> a[i][1];

        a[i][2] = i;
    }
    sort(all(a), [](auto &x, auto &y) {
        return x[0] < y[0] || (x[0] == y[0] && x[1] > y[1]);
    });

    // dbg(a);

    // for any j after i, if a[j].second <= a[i].second, then a[j] is nested in a[i]
    // for any j before i, if a[j].second >= a[i].second, then a[i] is nested in a[j]

    vector<int> contains(n), contained(n);
    
    int min_second = INT_MAX;
    for(int i = n-1; i >= 0; i--) {
        if (a[i][1] >= min_second) {
            contains[a[i][2]] = 1;
        }
        min_second = min(min_second, a[i][1]);
    }

    int max_second = INT_MIN;
    for(int i = 0; i < n; i++) {
        if (a[i][1] <= max_second) {
            contained[a[i][2]] = 1;
        }
        max_second = max(max_second, a[i][1]);
    }

    for(int i = 0; i < n; i++) {
        cout << contains[i] << " ";
    }
    cout << "\n";
    for(int i = 0; i < n; i++) {
        cout << contained[i] << " ";
    }
    cout << "\n";
    return 0;
}