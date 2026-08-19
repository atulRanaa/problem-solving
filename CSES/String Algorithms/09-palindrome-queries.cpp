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
 
using namespace std;
typedef long long ll;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define LCM(a,b) ((a*b)/__gcd(a,b))
#define inf 2e18
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

struct Trie {
private:
    int max_nodes, alphabet_size, pool_ptr = 1;
    vector<int> next_node, word_count, prefix_count;

    int alloc() {
        int id = pool_ptr++;
        return id;
    }
    
    inline int child(int node, char c) const {
        return node * alphabet_size + (c - 'a');
    }
public:
    Trie(int max, int alpha = 26) : max_nodes(max), alphabet_size(alpha) {
        next_node.assign(max * alpha, 0);
        word_count.assign(max, 0);
        prefix_count.assign(max, 0);
    }

    void insert(const string& s) {
        int u = 0;
        prefix_count[u]++;
        for (char c : s) {
            int cell = child(u, c);
            if (!next_node[cell]) next_node[cell] = alloc();
            u = next_node[cell];
            prefix_count[u]++;
        }
        word_count[u]++;
    }

    struct Cursor {
        const Trie& t;
        int u = 0;
        Cursor(const Trie& trie) : t(trie) {}

        bool next(char c) {
            int cell = t.child(u, c);
            if (!t.next_node[cell]) return false;
            u = t.next_node[cell];
            return true;
        }
        bool is_word()     const { return t.word_count[u] > 0; }
        bool is_prefix()   const { return t.prefix_count[u] > 0; }
        int count_prefix() const { return t.prefix_count[u]; }
        void reset() { u = 0; }
    };

    Cursor get_cursor() const { return Cursor(*this); }


    bool search(const string& s) const {
        auto state = get_cursor();
        for (char c : s) if (!state.next(c)) return false;
        return state.is_word();
    }
 
    bool startsWith(const string& prefix) const {
        auto state = get_cursor();
        for (char c : prefix) if (!state.next(c)) return false;
        return state.is_prefix();
    }
 
    int countPrefix(const string& prefix) const {
        auto state = get_cursor();
        for (char c : prefix) if (!state.next(c)) return 0;
        return state.count_prefix();
    }

};

vector<int> z_function(string s) {
    int n = s.size();
    vector<int> z(n);
    int l = 0, r = 0;
    for(int i = 1; i < n; i++) {
        if(i < r) {
            z[i] = min(r - i, z[i - l]);
        }
        while(i + z[i] < n && s[z[i]] == s[i + z[i]]) {
            z[i]++;
        }
        if(i + z[i] > r) {
            l = i;
            r = i + z[i];
        }
    }
    return z;
}

vector<int> suffix_array_construction(string const& s) {
    int n = s.size();
    const int alphabet = 256;
    vector<int> p(n), c(n), cnt(max(alphabet, n), 0);
    for (int i = 0; i < n; i++)
        cnt[s[i]]++;
    for (int i = 1; i < alphabet; i++)
        cnt[i] += cnt[i-1];
    for (int i = 0; i < n; i++)
        p[--cnt[s[i]]] = i;
    c[p[0]] = 0;
    int classes = 1;
    for (int i = 1; i < n; i++) {
        if (s[p[i]] != s[p[i-1]])
            classes++;
        c[p[i]] = classes - 1;
    }
    vector<int> pn(n), cn(n);
    for (int h = 0; (1 << h) < n; ++h) {
        for (int i = 0; i < n; i++) {
            pn[i] = p[i] - (1 << h);
            if (pn[i] < 0)
                pn[i] += n;
        }
        fill(cnt.begin(), cnt.begin() + classes, 0);
        for (int i = 0; i < n; i++)
            cnt[c[pn[i]]]++;
        for (int i = 1; i < classes; i++)
            cnt[i] += cnt[i-1];
        for (int i = n-1; i >= 0; i--)
            p[--cnt[c[pn[i]]]] = pn[i];
        cn[p[0]] = 0;
        classes = 1;
        for (int i = 1; i < n; i++) {
            pair<int, int> cur = {c[p[i]], c[(p[i] + (1 << h)) % n]};
            pair<int, int> prev = {c[p[i-1]], c[(p[i-1] + (1 << h)) % n]};
            if (cur != prev)
                ++classes;
            cn[p[i]] = classes - 1;
        }
        c.swap(cn);
    }
    return p;
}

// binary exponentiation
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


vector<int> prefix_function(string s) {
    int n = (int)s.length();
    vector<int> pi(n);
    for (int i = 1; i < n; i++) {
        int j = pi[i-1];
        while (j > 0 && s[i] != s[j])
            j = pi[j-1];
        if (s[i] == s[j])
            j++;
        pi[i] = j;
    }
    return pi;
}


struct fenwick_tree_mod {
  public:
    fenwick_tree_mod() : _n(0), mod(1) {}
    explicit fenwick_tree_mod(int n, ll m) : _n(n), mod(m), data(n, 0) {}

    void add(int p, ll x) {
        assert(0 <= p && p < _n);
        x %= mod;
        if (x < 0) x += mod;

        p++;
        while (p <= _n) {
            data[p - 1] = (data[p - 1] + x) % mod;
            p += p & -p;
        }
    }

    // prefix sum of [0, r), mod m
    ll sum(int r) {
        assert(0 <= r && r <= _n);
        ll s = 0;
        while (r > 0) {
            s = (s + data[r - 1]) % mod;
            r -= r & -r;
        }
        return s;
    }

    // range sum of [l, r), mod m
    ll sum(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        ll res = (sum(r) - sum(l) + mod) % mod;
        if (res < 0) res += mod;
        return res;
    }

  private:
    int _n;
    ll mod;
    vector<ll> data;
};

const ll base = 137LL;

int main() {
    fast;

    int n, m;
    cin >> n >> m;
    string s; cin >> s;


    fenwick_tree_mod fw(n + 1, MOD);
    fenwick_tree_mod bw(n + 1, MOD);

    // Hash(S) = SUM (s[i] * p ^ i) (mod M) for i in range [0, n-1];

    vector<ll> p_exponent(n + 1, 1);
    for(int i = 0; i < n; i++)
        p_exponent[i+1] = (p_exponent[i] * base) % MOD;

    for(int i = 0; i < n; i++) {
        fw.add(i, s[i] * p_exponent[i]);
        bw.add(i, s[i] * p_exponent[n-1-i]);
    }


    int query_t;
    int k;
    char ch;
    while(m-- > 0) {
        cin >> query_t;

        if(query_t == 1) {
            cin >> k >> ch;
            --k;
            fw.add(k, (ch - s[k]) * p_exponent[k]);
            bw.add(k, (ch - s[k]) * p_exponent[n-1-k]);

            s[k] = ch; 
        } else {
            int a, b;
            cin >> a >> b;
            --a;
            --b;
            

            ll forward = fw.sum(a, b + 1) * p_exponent[n-1-b] % MOD;
            ll backward = bw.sum(a, b + 1) * p_exponent[a] % MOD;

            cout << ((forward == backward)?"YES":"NO") << "\n";
        }
    }
    return 0;
}