#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const int INF = 1e9;

// ═══════════════════════════════════════════════════════
//  Suffix Array Template
//
//  Build:   O(n log n)  — DC3/radix sort doubling
//  LCP:     O(n)        — Kasai's algorithm
//  RMQ:     O(n log n)  — sparse table for O(1) LCP queries
//
//  Supports:
//   - SA + LCP construction            → SuffixArray(s)
//   - LCP of suffix i and j            → lcp(i, j)       O(1)
//   - Pattern occurrences (count+pos)  → find(pattern)   O(m log n)
//   - Longest repeated substring       → longest_repeated()
//   - Longest common substring         → lcs(t)
//   - Count distinct substrings        → count_distinct()
//   - Lexicographic rank of suffix i   → rank[i]
//   - kth lexicographic suffix         → sa[k]
//   - Concatenated string queries      → lcs of many strings
// ═══════════════════════════════════════════════════════

struct SuffixArray {
    int n;
    string s;
    vector<int> sa;    // sa[i] = start of i-th lexicographically smallest suffix
    vector<int> rank_; // rank_[i] = rank of suffix starting at i (inverse of sa)
    vector<int> lcp_;  // lcp_[i] = LCP of sa[i] and sa[i-1] (Kasai)
                       // lcp_[0] = 0 by convention

    // ── Sparse table for O(1) range minimum (for LCP queries) ────────
    vector<vector<int>> sparse;
    vector<int> log2_;

    // ────────────────────────────────────────────────────────────────
    //  1. Suffix Array Construction — O(n log n) radix doubling
    //     Builds sa[] and rank_[] simultaneously
    // ────────────────────────────────────────────────────────────────
    vector<int> build_sa(const string& s) {
        int n = s.size();
        const int alphabet = 256;
        vector<int> p(n), c(n), cnt(max(alphabet, n), 0);

        // Initial sort by single characters
        for (int i = 0; i < n; i++) cnt[(unsigned char)s[i]]++;
        for (int i = 1; i < alphabet; i++) cnt[i] += cnt[i-1];
        for (int i = 0; i < n; i++) p[--cnt[(unsigned char)s[i]]] = i;

        c[p[0]] = 0;
        int classes = 1;
        for (int i = 1; i < n; i++) {
            if (s[p[i]] != s[p[i-1]]) classes++;
            c[p[i]] = classes - 1;
        }

        vector<int> pn(n), cn(n);
        for (int h = 0; (1 << h) < n; h++) {
            // Sort by second half (shift left by 2^h)
            for (int i = 0; i < n; i++) {
                pn[i] = p[i] - (1 << h);
                if (pn[i] < 0) pn[i] += n;
            }

            // Stable sort by first half class
            fill(cnt.begin(), cnt.begin() + classes, 0);
            for (int i = 0; i < n; i++) cnt[c[pn[i]]]++;
            for (int i = 1; i < classes; i++) cnt[i] += cnt[i-1];
            for (int i = n-1; i >= 0; i--) p[--cnt[c[pn[i]]]] = pn[i];

            // Recompute classes for pairs
            cn[p[0]] = 0;
            classes = 1;
            for (int i = 1; i < n; i++) {
                pair<int,int> cur  = {c[p[i]],   c[(p[i]   + (1 << h)) % n]};
                pair<int,int> prev = {c[p[i-1]], c[(p[i-1] + (1 << h)) % n]};
                if (cur != prev) classes++;
                cn[p[i]] = classes - 1;
            }
            c.swap(cn);
        }
        return p;
    }

    // ────────────────────────────────────────────────────────────────
    //  2. LCP Array — Kasai's Algorithm O(n)
    //     lcp_[i] = longest common prefix of suffix sa[i] and sa[i-1]
    //     Uses the key insight: lcp(sa[rank[i]], sa[rank[i]-1]) ≥ lcp(sa[rank[i-1]], sa[rank[i-1]-1]) - 1
    // ────────────────────────────────────────────────────────────────
    vector<int> build_lcp(const string& s, const vector<int>& sa,
                           const vector<int>& rank_) {
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

    // ────────────────────────────────────────────────────────────────
    //  3. Sparse Table for O(1) RMQ on LCP array
    //     lcp_query(i, j) = min(lcp_[i+1..j]) for i < j (SA indices)
    //     → equals LCP of suffix sa[i] and sa[j]
    // ────────────────────────────────────────────────────────────────
    void build_sparse() {
        log2_.assign(n + 1, 0);
        for (int i = 2; i <= n; i++) log2_[i] = log2_[i/2] + 1;
        int LOG = log2_[n] + 1;
        sparse.assign(LOG, vector<int>(n));
        sparse[0] = lcp_;
        for (int j = 1; j < LOG; j++)
            for (int i = 0; i + (1 << j) <= n; i++)
                sparse[j][i] = min(sparse[j-1][i], sparse[j-1][i + (1 << (j-1))]);
    }

    // RMQ on lcp_ in range [l, r] (closed)
    int rmq(int l, int r) const {
        if (l > r) return INF;
        int k = log2_[r - l + 1];
        return min(sparse[k][l], sparse[k][r - (1 << k) + 1]);
    }

    // ── Constructor ───────────────────────────────────────────────────
    explicit SuffixArray(const string& str) : n(str.size()), s(str) {
        sa     = build_sa(s);
        rank_.resize(n);
        for (int i = 0; i < n; i++) rank_[sa[i]] = i;
        lcp_   = build_lcp(s, sa, rank_);
        build_sparse();
    }

    // ────────────────────────────────────────────────────────────────
    //  4. LCP of two SUFFIXES (by their starting positions in s)
    //     lcp_suffix(i, j) in O(1)
    // ────────────────────────────────────────────────────────────────
    int lcp_suffix(int i, int j) const {
        if (i == j) return n - i;
        int ri = rank_[i], rj = rank_[j];
        if (ri > rj) swap(ri, rj);
        return rmq(ri + 1, rj);   // min LCP in range between their SA positions
    }

    // ────────────────────────────────────────────────────────────────
    //  5. Pattern Search — O(m log n)
    //     Returns [lo, hi) range in SA where pattern matches
    //     → hi - lo = number of occurrences
    // ────────────────────────────────────────────────────────────────
    pair<int,int> search_range(const string& pat) const {
        int m = pat.size();
        // Lower bound: first suffix >= pat
        int lo = 0, hi = n;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (s.compare(sa[mid], m, pat) < 0) lo = mid + 1;
            else                                hi = mid;
        }
        int left = lo;
        // Upper bound: first suffix > pat
        hi = n;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (s.compare(sa[mid], m, pat) <= 0)    lo = mid + 1;
            else                                    hi = mid;
        }
        return {left, lo};
    }

    // Count occurrences of pattern in s
    int count(const string& pat) const {
        auto [l, r] = search_range(pat);
        return r - l;
    }

    // All starting positions of pattern in s (unsorted)
    vector<int> find_all(const string& pat) const {
        auto [l, r] = search_range(pat);
        vector<int> res;
        for (int i = l; i < r; i++) res.push_back(sa[i]);
        return res;
    }

    // ────────────────────────────────────────────────────────────────
    //  6. Longest Repeated Substring — O(n)
    //     = maximum value in lcp_ array
    //     If lcp_[i] = k, then sa[i-1] and sa[i] share a length-k prefix
    // ────────────────────────────────────────────────────────────────
    pair<int,int> longest_repeated() const {
        int best = 0, best_i = 0;
        for (int i = 1; i < n; i++)
            if (lcp_[i] > best) { best = lcp_[i]; best_i = sa[i]; }
        return {best_i, best};   // {start, length}
    }

    // ────────────────────────────────────────────────────────────────
    //  7. Count Distinct Substrings — O(n)
    //     Total substrings = n*(n+1)/2
    //     Subtract sum of LCP array (these are duplicates)
    // ────────────────────────────────────────────────────────────────
    ll count_distinct() const {
        ll total = (ll)n * (n + 1) / 2;
        for (int i = 1; i < n; i++) total -= lcp_[i];
        return total;
    }

    // ────────────────────────────────────────────────────────────────
    //  8. Longest Common Substring of s and t — O((n+m) log(n+m))
    //     Concatenate s + '#' + t, build SA, find max LCP between
    //     suffixes from different strings
    //     '#' must be < any character in s and t
    // ────────────────────────────────────────────────────────────────
    static pair<string,int> lcs(const string& a, const string& b) {
        string combined = a + '#' + b;
        SuffixArray sa_combined(combined);
        int na = a.size(), n_combined = combined.size();

        int best = 0, best_pos = 0;
        for (int i = 1; i < n_combined; i++) {
            int u = sa_combined.sa[i-1];
            int v = sa_combined.sa[i];
            bool u_in_a = (u < na);
            bool v_in_a = (v < na);
            if (u_in_a != v_in_a) {   // one from each string
                int l = sa_combined.lcp_[i];
                if (l > best) { best = l; best_pos = min(u, v); }
            }
        }
        return {combined.substr(best_pos, best), best};
    }

    // ────────────────────────────────────────────────────────────────
    //  9. Generalized LCS for k strings — O(N log N) where N = total len
    //     Concatenate all strings with unique separators
    //     Find longest prefix shared by at least k strings
    // ────────────────────────────────────────────────────────────────
    static string lcs_k(const vector<string>& strs, int k) {
        string combined;
        vector<int> which(0);   // which[i] = string index containing position i
        int idx = 0;
        for (int si = 0; si < (int)strs.size(); si++) {
            for (char c : strs[si]) {
                combined += c;
                which.push_back(si);
            }
            if (si + 1 < (int)strs.size()) {
                combined += (char)(1 + si);  // unique separator < 'a'
                which.push_back(-1);         // separator belongs to no string
            }
        }

        SuffixArray sa_obj(combined);
        int n_combined = combined.size();
        int best_len = 0, best_pos = 0;

        // Sliding window on SA: find window where k distinct strings appear
        // and minimum LCP in that window is maximized
        // Binary search on answer length + sliding window
        auto check = [&](int len) -> int {
            // find leftmost position where window has >= k distinct strings
            // and min lcp in window >= len
            // Sliding window: expand right while min lcp >= len, track distinct
            set<int> in_window;
            int l = 0;
            for (int r = 0; r < n_combined; r++) {
                if (r > 0 && sa_obj.lcp_[r] < len) {
                    l = r; in_window.clear();
                }
                if (which[sa_obj.sa[r]] != -1)
                    in_window.insert(which[sa_obj.sa[r]]);
                if ((int)in_window.size() >= k) return sa_obj.sa[l];
            }
            return -1;
        };

        int lo = 0, hi = (int)strs[0].size();
        for (auto& str : strs) hi = min(hi, (int)str.size());
        while (lo < hi) {
            int mid = lo + (hi - lo + 1) / 2;
            if (check(mid) != -1) { lo = mid; best_pos = check(mid); }
            else                    hi = mid - 1;
        }
        return combined.substr(best_pos, lo);
    }

    // ────────────────────────────────────────────────────────────────
    //  10. kth Lexicographic Distinct Substring — O(n)
    //      Uses the fact that suffix sa[i] contributes (n - sa[i]) - lcp_[i] new
    //      distinct substrings in sorted order
    // ────────────────────────────────────────────────────────────────
    string kth_distinct_substr(ll k) const {
        for (int i = 0; i < n; i++) {
            ll new_substrs = (n - sa[i]) - (i > 0 ? lcp_[i] : 0);
            if (k <= new_substrs)
                return s.substr(sa[i], lcp_[i < 1 ? 0 : i] + k);
            k -= new_substrs;
        }
        return "";   // k too large
    }

    // ────────────────────────────────────────────────────────────────
    //  11. Suffix Array of a rotation (circular string)
    //      Duplicate the string s+s, build SA, filter sa[i] < n
    // ────────────────────────────────────────────────────────────────
    static vector<int> rotation_sa(const string& s) {
        SuffixArray sa_obj(s + s);
        vector<int> result;
        int n = s.size();
        for (int x : sa_obj.sa)
            if (x < n) result.push_back(x);
        return result;
    }

    // ── Debug ─────────────────────────────────────────────────────────
    void print() const {
        cerr << "SA | LCP | Suffix\n";
        for (int i = 0; i < n; i++)
            cerr << sa[i] << "  | " << lcp_[i] << "   | " << s.substr(sa[i]) << "\n";
    }
};

// ────────────────────────────────────────────────────────
//  Suffix Automaton (SAM) — O(n) build
//  Recognizes all substrings; complement to Suffix Array
//  Use when: counting occurrences online, streaming, DAG queries
// ────────────────────────────────────────────────────────
struct SuffixAutomaton {
    struct State {
        map<char,int> next;
        int link, len;
        ll  cnt;   // number of times this state is visited (= occurrences)
    };

    vector<State> st;
    int last;

    SuffixAutomaton() {
        st.push_back({{}, -1, 0, 0});
        last = 0;
    }

    void extend(char c) {
        int cur = st.size();
        st.push_back({{}, -1, st[last].len + 1, 1});
        int p = last;
        while (p != -1 && !st[p].next.count(c)) {
            st[p].next[c] = cur; p = st[p].link;
        }
        if (p == -1) {
            st[cur].link = 0;
        } else {
            int q = st[p].next[c];
            if (st[p].len + 1 == st[q].len) {
                st[cur].link = q;
            } else {
                int clone = st.size();
                st.push_back({st[q].next, st[q].link, st[p].len + 1, 0});
                while (p != -1 && st[p].next.count(c) && st[p].next[c] == q) {
                    st[p].next[c] = clone; p = st[p].link;
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
    }

    // Count occurrences of pattern in the original string
    ll count(const string& pat) const {
        int cur = 0;
        for (char c : pat) {
            auto it = st[cur].next.find(c);
            if (it == st[cur].next.end()) return 0;
            cur = it->second;
        }
        return st[cur].cnt;
    }

    ll count_distinct() const {
        ll res = 0;
        for (int i = 1; i < (int)st.size(); i++)
            res += st[i].len - st[st[i].link].len;
        return res;
    }
};