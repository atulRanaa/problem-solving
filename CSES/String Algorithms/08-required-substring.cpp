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

struct Manacher {
    int n;
    string s;
    vector<int> odd, even;

    explicit Manacher(const string& s) : n(s.size()), s(s) {
        build();
    }

    void build() {
        build_odd();
        build_even();
    }

    // Odd-length palindromes — centered at each character
    void build_odd() {
        odd.assign(n, 1);
        int l = 0, r = 0;   // current rightmost palindrome: [l, r]
        for (int i = 0; i < n; i++) {
            // Mirror of i with respect to center (l+r)/2
            if (i < r) odd[i] = min(odd[l + r - i], r - i + 1);
            // Try to expand
            while (i - odd[i] >= 0 && i + odd[i] < n && s[i - odd[i]] == s[i + odd[i]]) {
                odd[i]++;
            }
            // Update rightmost palindrome
            if (i + odd[i] - 1 > r) { 
                l = i - odd[i] + 1; 
                r = i + odd[i] - 1; 
            }
        }
    }

    // Even-length palindromes — centered between i-1 and i
    void build_even() {
        even.assign(n, 0);
        int l = 0, r = -1;
        for (int i = 0; i < n; i++) {
            if (i <= r) even[i] = min(even[l + r - i + 1], r - i + 1);
            while (i - even[i] - 1 >= 0 && i + even[i] < n && s[i - even[i] - 1] == s[i + even[i]]) {
                even[i]++;
            }
            if (i + even[i] - 1 > r) { 
                l = i - even[i]; 
                r = i + even[i] - 1; 
            }
        }
    }

    // Is s[l..r] (0-indexed, inclusive) a palindrome?
    bool is_palindrome(int l, int r) const {
        int len = r - l + 1;
        int mid = (l + r) / 2;
        if (len % 2 == 1) {
            int rad = (len + 1) / 2;
            return odd[mid] >= rad;
        } else {
            int center = r - len / 2 + 1;  // first right-of-center index
            int rad = len / 2;
            return even[center] >= rad;
        }
    }

    // Returns {start, length} of longest palindrome in s
    pair<int,int> longest_palindrome() const {
        int best_len = 1, best_pos = 0;
        // Check odd palindromes
        for (int i = 0; i < n; i++) {
            int len = 2 * odd[i] - 1;
            if (len > best_len) { best_len = len; best_pos = i - odd[i] + 1; }
        }
        // Check even palindromes
        for (int i = 0; i < n; i++) {
            int len = 2 * even[i];
            if (len > best_len) { best_len = len; best_pos = i - even[i]; }
        }
        return {best_pos, best_len};
    }

    // Total count of (l, r) pairs where s[l..r] is palindrome
    ll count_palindromes() const {
        ll cnt = 0;
        for (int i = 0; i < n; i++) cnt += odd[i];   // odd centered at i
        for (int i = 0; i < n; i++) cnt += even[i];  // even centered between i-1,i
        return cnt;
    }

    // Returns list of {center_index, radius, is_odd}
    // for every maximal palindrome (one per center)
    struct PalCenter { int center, radius; bool is_odd; };
    vector<PalCenter> all_palindromes() const {
        vector<PalCenter> result;
        for (int i = 0; i < n; i++)
            result.push_back({i, odd[i], true});
        for (int i = 0; i < n; i++)
            if (even[i] > 0)
                result.push_back({i, even[i], false});
        return result;
    }

    // Count distinct palindromic substrings s[l..r] where l<=p<=r
    ll count_containing(int p) const {
        ll cnt = 0;
        for (int i = 0; i < n; i++) {
            // Odd: center i, covers [i-odd[i]+1 .. i+odd[i]-1]
            // How many radii r such that l<=p<=r?
            // l = i - r + 1 <= p → r >= i - p + 1
            // r = i + r - 1 >= p → r >= p - i + 1
            int min_rad = max(abs(i - p) + 1, 1);
            cnt += max(0, odd[i] - min_rad + 1);
        }
        for (int i = 0; i < n; i++) {
            // Even: center between i-1 and i, covers [i-r .. i+r-1]
            int min_rad = max(i - p, p - i + 1);
            cnt += max(0, even[i] - min_rad + 1);
        }
        return cnt;
    }

    // Longest l such that s[0..l-1] is a palindrome
    int longest_pal_prefix() const {
        int best = 1;
        for (int i = 0; i < n; i++) {
            // Odd: center i, left end = i - odd[i] + 1 = 0 → odd[i] = i+1
            if (odd[i] == i + 1) best = max(best, 2 * i + 1);
            // Even: center between i-1 and i, left end = i - even[i] = 0
            if (even[i] == i)    best = max(best, 2 * i);
        }
        return best;
    }

    // Longest l such that s[n-l..n-1] is a palindrome
    int longest_pal_suffix() const {
        int best = 1;
        for (int i = 0; i < n; i++) {
            // Odd: right end = i + odd[i] - 1 = n-1 → odd[i] = n-i
            if (odd[i] == n - i) best = max(best, 2 * (n - i) - 1);
            // Even: right end = i + even[i] - 1 = n-1 → even[i] = n-i
            if (even[i] == n - i) best = max(best, 2 * (n - i));
        }
        return best;
    }

    // Minimum number of cuts to partition s into palindromes
    vector<int> min_cuts() const {
        vector<int> dp(n + 1, INT_MAX);
        dp[0] = -1;
        for (int i = 1; i <= n; i++) {
            for (int j = 0; j < i; j++) {
                if (dp[j] != INT_MAX && is_palindrome(j, i - 1))
                    dp[i] = min(dp[i], dp[j] + 1);
            }
        }
        return dp;   // answer is dp[n]
    }

    // Returns lengths of all palindromic suffixes of s[0..i]
    vector<vector<int>> all_pal_suffixes() const {
        vector<vector<int>> result(n);
        for (int i = 0; i < n; i++) {
            // Odd palindromes ending at i: center c, right = c + odd[c] - 1 = i
            // → c + odd[c] = i + 1 → need to scan (expensive naively)
            // For O(n): use palindromic series — see eertree for O(n) version
            for (int c = 0; c < n; c++) {
                if (c + odd[c] - 1 == i)
                    result[i].push_back(2 * odd[c] - 1);
                if (c + even[c] - 1 == i && even[c] > 0)
                    result[i].push_back(2 * even[c]);
            }
        }
        return result;
    }

    // How many times does the palindrome s[l..r] appear in s?
    int count_occurrences(int l, int r) const {
        int cnt = 0, len = r - l + 1;
        for (int i = 0; i + len - 1 < n; i++)
            if (is_palindrome(i, i + len - 1) && is_palindrome(l, r))
                // same length palindromes: check using odd/even arrays
                cnt++;
        return cnt;
    }

    vector<int> longest_pal_ending() const {
        vector<int> start(n), arr(n, 1);
        for(int i = 0; i < n; i++)
            start[i] = i;

        int l, r;
        for(int i = 0; i < n; i++) {
            l = i - odd[i] + 1, r = i + odd[i] - 1;
            start[r] = min(start[r], l);

            l = i - even[i], r = i + even[i] - 1;
            start[r] = min(start[r], l);
        }

        for(int i = n-1; i > 0; i--) {
            start[i-1] = min(start[i-1], start[i] + 1);
        }
        for(int i = 0; i < n; i++) {
            arr[i] = i - start[i] + 1;
        }

        return arr;
    }

    // Debug: print odd/even arrays
    void print() const {
        cerr << "s   : "; for (char x : s)  cerr << x << " "; cerr << "\n";
        cerr << "odd : "; for (int x : odd)  cerr << x << " "; cerr << "\n";
        cerr << "even: "; for (int x : even) cerr << x << " "; cerr << "\n";
    }
};

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


int main() {
    fast;

    int n;
    cin >> n;
    string s;
    cin >> s;

    int m = s.size();
    
    if(n < m) {
        cout << 0 << "\n";
        return 0;
    }
    if(n == m) {
        cout << 1 << "\n";
        return 0;
    }

    vector<int> prefix = prefix_function(s);
    vector< vector<int> > transition(m + 1, vector<int> (26, 0));
    vector< vector<ll> > dp(n + 1, vector<ll> (m + 1, 0));
    dp[0][0] = 1;

    for(int j = 0; j < m; j++) {
        for(int c = 0; c < 26; c++) {
            if(s[j] == (char)(c + 'A')) {
                transition[j][c] = j+1;
            } else {
                if(j-1 >=0) transition[j][c] = transition[prefix[j-1]][c];
                else        transition[j][c] = 0;
            }
        }
    }
    for(int c = 0; c < 26; c++) transition[m][c] = m;

    for(int i = 0; i < n; i++) {
        for(int j = 0; j <= m; j++) {
            for(int c = 0; c < 26; c++) {
                int _j = transition[j][c];
                dp[i+1][_j] += dp[i][j];
                dp[i+1][_j] %= MOD;
            }
        }
    }

    // dbg(prefix);
    // dbg(transition);
    // dbg(dp);
    cout << dp[n][m] << "\n";

    return 0;
}