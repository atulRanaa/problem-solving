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

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace std;
using namespace __gnu_pbds;

typedef long long ll;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define LCM(a,b) ((a*b)/__gcd(a,b))
#define inf 1e15
#define test ll cse; cin >> cse; for(ll _i = 1; _i <= cse; _i++)
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

template <typename T>
struct pbds_tree {
    tree<
        pair<T, int>, 
        null_type, 
        less<pair<T, int>>, 
        rb_tree_tag, 
        tree_order_statistics_node_update
    > s;
    int timer = 0; // Incremental unique tracker ID

    void insert(T val) {
        s.insert({val, ++timer});
    }

    bool erase(T val) {
        auto it = s.lower_bound({val, 0});
        if (it != s.end() && it->first == val) {
            s.erase(it);
            return true;
        }
        return false;
    }

    // Count numbers strictly less than val
    int count_less_than(T val) {
        return s.order_of_key({val, 0});
    }

    // Count numbers less than or equal to val
    int count_less_equal(T val) {
        // Using a high marker ensures we sit past all elements matching 'val'
        return s.order_of_key({val, 2e9}); 
    }

    // Find the k-th smallest element (0-indexed)
    T find_kth(int k) {
        if (k < 0 || k >= (int)s.size()) return -1; // Out of bounds check
        return s.find_by_order(k)->first;
    }

    int size() { return s.size(); }
};


int main() {
    fast;

    ll n, q; cin >> n >> q;
    
    vector<ll> arr(n + 1);
    pbds_tree<ll> tree;

    for(int i = 1; i <= n; i++) {
        cin >> arr[i];

        tree.insert(arr[i]);
    }

    ll a, b;
    char ch;
    for(int i = 1; i <= q; i++) {
        cin >> ch >> a >> b;

        if(ch == '?') {
            ll x = tree.count_less_equal(b);
            ll y = tree.count_less_than(a);

            cout << x - y << "\n"; 
        } else {
            tree.erase(arr[a]);
            tree.insert(b);
            arr[a] = b;
        }
    }
    cout << "\n";
    return 0;
}