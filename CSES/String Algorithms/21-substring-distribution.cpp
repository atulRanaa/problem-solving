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

struct SuffixAutomaton {
private:
    struct State {
        map<char,int> next;
        int link, len;
        ll  cnt; // occurrences

        int firstpos;
    };

    vector<ll> paths;
    vector<State> st;
    int last;

public:
    SuffixAutomaton() {
        st.push_back({{}, -1, 0, 0, -1});
        last = 0;
    }

    void extend(char c) {
        int cur = st.size();
        st.push_back({{}, -1, st[last].len + 1, 1, st[last].len});
        int p = last;
        while (p != -1 && !st[p].next.count(c)) {
            st[p].next[c] = cur; 
            p = st[p].link;
        }
        if (p == -1) {
            st[cur].link = 0;
        } else {
            int q = st[p].next[c];
            if (st[p].len + 1 == st[q].len) {
                st[cur].link = q;
            } else {
                int clone = st.size();
                st.push_back({st[q].next, st[q].link, st[p].len + 1, 0, st[q].firstpos});
                while (p != -1 && st[p].next.count(c) && st[p].next[c] == q) {
                    st[p].next[c] = clone; 
                    p = st[p].link;
                }
                st[q].link = st[cur].link = clone;
            }
        }
        last = cur;
    }

    void build(const string& s) {
        for (char c : s) extend(c);
        // Propagate counts in topological order (by len, decreasing)
        int m = st.size();
        vector<int> order(m); 
        iota(order.begin(), order.end(), 0);
        sort(order.begin(), order.end(), [&](int a, int b) {
            return st[a].len > st[b].len;
        });
        for (int v : order)
            if (st[v].link != -1) st[st[v].link].cnt += st[v].cnt;

        
        paths.assign(m, 0);
        for(int u: order) {
            paths[u] = st[u].cnt;
            for(auto [c, v]: st[u].next) {
                paths[u] += paths[v];
            }
        }
        paths[0] = 0;
        for (auto [c, v] : st[0].next) {
            paths[0] += paths[v];
        }
    }

    ll count_distinct() const {
        ll res = 0;
        for (int i = 1; i < (int)st.size(); i++)
            res += st[i].len - st[st[i].link].len;
        return res;
    }

    void count_distinct_substr(int n) {
        vector<ll> prefix(n, 0);
        ll res = 0;
        for (int i = 1; i < (int)st.size(); i++) {

            prefix[st[st[i].link].len] += 1;
            prefix[st[i].len] -= 1;

            res += st[i].len - st[st[i].link].len;
        }

        for(int i = 1; i <= n; i++) 
            prefix[i] += prefix[i-1];

        for(auto& x: prefix) {
            cout << x << " ";
        }
        cout << "\n";
    }

    string solve(ll k) {
        int curr = 0;
        string ans;
        while(k > 0) {
            for(auto [c, p]: st[curr].next) {
                if(paths[p] < k) {
                    k -= paths[p];
                } else {
                    ans += c;
                    
                    k -= st[p].cnt;
                    if(k <= 0) {
                        return ans;
                    }
                    curr = p;
                    break;
                }
            }
        }

        return ans;
    }
};


struct SuffixArray {
public:
    int n;
    string s;
    vector<int> sa;   
    vector<int> rank_;
    vector<int> lcp_; 

    vector<vector<int>> sparse;
    vector<int> log2_;

    vector<int> sort_cyclic_shifts(string const& s) {
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

    vector<int> build_sa(string s) {
        s += "$";
        vector<int> sorted_shifts = sort_cyclic_shifts(s);
        sorted_shifts.erase(sorted_shifts.begin());
        return sorted_shifts;
    }

    // insight: lcp(sa[rank[i]], sa[rank[i]-1]) ≥ lcp(sa[rank[i-1]], sa[rank[i-1]-1]) - 1
    vector<int> build_lcp(const string& s, const vector<int>& sa, const vector<int>& rank_) {
        int n = s.size();
        vector<int> lcp(n, 0);
        int h = 0;
        for (int i = 0; i < n; i++) {
            if (rank_[i] > 0) {
                int j = sa[rank_[i] - 1];
                while (i + h < n && j + h < n && s[i+h] == s[j+h]) h++;
                lcp[rank_[i]] = h;
                if (h > 0) h--;
            }
        }
        return lcp;
    }

public:
    explicit SuffixArray(const string& str) : n(str.size()), s(str) {
        sa = build_sa(s);
        rank_.resize(n);
        for (int i = 0; i < n; i++) rank_[sa[i]] = i;
        lcp_ = build_lcp(s, sa, rank_);
    }

    string kth_distinct_substr(ll k) const {
        for (int i = 0; i < n; i++) {
            ll new_substrs = (n - sa[i]) - (i > 0 ? lcp_[i] : 0);
            if (k <= new_substrs)
                return s.substr(sa[i], lcp_[i < 1? 0 : i] + k);
            k -= new_substrs;
        }
        return "";   // k too large
    }

    string kth_substr(ll k) const {
        for (int i = 0; i < n; i++) {
            ll new_substrs = (n - sa[i]);
            if (k <= new_substrs)
                return s.substr(sa[i], k);
            k -= new_substrs;
        }
        return "";
    }

    void print() const {
        cerr << "SA | LCP | Suffix\n";
        for (int i = 0; i < n; i++)
            cerr << sa[i] << "  | " << lcp_[i] << "   | " << s.substr(sa[i]) << "\n";
    }
};

int main() {
    fast;

    string s; cin >> s;
    int n = s.size();

    SuffixAutomaton sa;
    
    sa.build(s);
    sa.count_distinct_substr(n);

    return 0;
}