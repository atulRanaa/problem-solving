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

int main() {
    fast;

    const int N = 1e6 + 10;
    string s;
    cin >> s;

    int k;
    cin >> k;
    
    string x;
    Trie tree(N, 26);
    for(int i = 0; i < k; i++) {
        cin >> x;
        tree.insert(x);
    }

    vector<ll> dp(s.size() + 1, 0);
    int n = s.size();
    dp[0] = 1;
    for(int i = 0; i < n; i++) {
        if (dp[i] == 0) continue; 

        auto state = tree.get_cursor();
        for(int j = i; j < n; j++) {
            if(!state.next(s[j]))
                break;

            if(state.is_word()) {
                dp[j + 1] += dp[i];
                dp[j + 1] %= MOD;
            }
        }
    }

    cout << dp[s.size()] << "\n";

    return 0;
}