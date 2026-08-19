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
#include <random>
#include <chrono>

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

mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
ll rand_ll() { return uniform_int_distribution<ll>(0, 1e18)(rng); }


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

template <class T=ll>
struct implicit_treap {
private:

    struct Node {
        T val;
        T sum;
        int sz;
        ll pri;
        int left, right;
        bool rev;
    };

    int root = 0;
    int pool_ptr = 1; 
    vector<Node> pool;

    int nd_sz (int t) { return t ? pool[t].sz  : 0; }
    T nd_sum (int t) { return t ? pool[t].sum  : 0; }
    int new_node(T val) {
        int idx = pool_ptr++;
        pool[idx] = {val, val, 1, rand_ll(), 0, 0, false};
        return idx;
    }

    void pull(int t) {
        if (!t) return;
        auto& n = pool[t];
        int l = n.left, r = n.right;
        n.sz  = 1 + nd_sz(l) + nd_sz(r);
        n.sum = n.val + nd_sum(l) + nd_sum(r); 
    }

    void push(int t) {
        if (!t) return;
        auto& n = pool[t];

        if(n.rev) {
            apply_rev(n.left); apply_rev(n.right);
            n.rev = false;
        }
    } 

    void split(int t, int k, int& l, int& r) {
        if(!t) { l = r = 0; return;}
        push(t);
        int ls = nd_sz(pool[t].left);
        if(ls >= k) {
            split(pool[t].left, k, l, pool[t].left);
            r = t;
        } else {
            split(pool[t].right, k - ls - 1, pool[t].right, r);
            l = t;
        }
        pull(t);
    }

    int merge(int l, int r) {
        if (!l || !r) return l + r;
        push(l);; push(r);

        if (pool[l].pri > pool[r].pri) {
            pool[l].right = merge(pool[l].right, r);
            pull(l); return l;
        } else {
            pool[r].left = merge(l, pool[r].left);
            pull(r); return r;
        }
    }

    void walk(int t, vector<T>& out) {
        if (!t) return;
        push(t);

        walk(pool[t].left, out);
        out.push_back(pool[t].val);
        walk(pool[t].right, out);
    }

    void apply_rev(int t) {
        if (!t) return;
        swap(pool[t].left, pool[t].right);
        pool[t].rev ^= 1;
    }

    tuple<int, int, int> isolate(int l, int r) {
        int t1, t2, t3;
        split(root, l, t1, t2);
        split(t2, r - l + 1, t2, t3);
        return {t1, t2, t3};
    }

public:
    implicit_treap(int maxn) {
        pool.resize(maxn);
    }

    int size() { return nd_sz(root); }

    void clear() {
        pool_ptr = 1;
        root = 0;
    }

    vector<T> to_array() {
        vector<T> v; 
        walk(root, v); return v;
    }

    void push_back(T val) { root = merge(root, new_node(val)); }

    void move_to(int l, int r, int pos) {
        int t1, t2, t3;
        split(root, l, t1, t2);
        split(t2, r - l + 1, t2, t3);
        // Now t2 is the subarray, pos must be adjusted
        int r1, r2;
        int combined = merge(t1, t3);
        int new_pos = nd_sz(combined); 
        split(combined, new_pos, r1, r2);
        root = merge(merge(r1, t2), r2);
    }

    void reverse(int l, int r) {
        int t1, t2, t3;
        split(root, l, t1, t2);
        split(t2, r - l + 1, t2, t3);

        apply_rev(t2);
        root = merge(merge(t1, t2), t3);
    }

    T query_sum(int l, int r) {
        auto [t1, t2, t3] = isolate(l, r);
        T v = nd_sum(t2);
        root = merge(merge(t1, t2), t3);
        
        return v;
    }
};

int main() {
    fast;

    ll n, m; cin >> n >> m;

    const int N = 4e5 + 10;
    implicit_treap<ll> tree(N);
    ll x;
    for(int i = 0; i < n; i++) {
        cin >> x;
        tree.push_back(x);
    }
    int query_t, a, b;
    while(m-- > 0) {
        cin >> query_t >> a >> b;
        --a;
        --b;

        if(query_t == 1)
            tree.reverse(a, b);
        else {
            cout << tree.query_sum(a, b) << "\n";
        }
    }
    return 0;
}
