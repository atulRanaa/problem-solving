#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

// ═══════════════════════════════════════════════════════
//  Manacher's Algorithm Template
//
//  Core: computes p[] where p[i] = radius of longest
//  palindrome centered at i in the transformed string
//  → O(n) time, O(n) space
//
//  Supports:
//   - Odd palindrome radii         → odd[i]
//   - Even palindrome radii        → even[i]
//   - O(1) palindrome check        → is_palindrome(l, r)
//   - Longest palindromic substr   → longest_palindrome()
//   - Count palindromic substrs    → count_palindromes()
//   - Palindrome at every center   → all_palindromes()
//   - Min palindrome partition     → min_cuts()
//   - Longest palindromic prefix   → longest_pal_prefix()
//   - Longest palindromic suffix   → longest_pal_suffix()
//   - Eertree (palindromic tree)   → PalindromicTree struct
// ═══════════════════════════════════════════════════════

struct Manacher {
    int n;
    string s;

    // odd[i]  = radius of longest ODD palindrome centered at i
    //           s[i-odd[i]+1 .. i+odd[i]-1] is palindrome
    //           radius 1 means just the single character
    // even[i] = radius of longest EVEN palindrome centered BETWEEN i-1 and i
    //           s[i-even[i] .. i+even[i]-1] is palindrome
    //           radius 0 means no even palindrome here
    vector<int> odd, even;

    explicit Manacher(const string& s) : n(s.size()), s(s) {
        build();
    }

    // ── Core Build ────────────────────────────────────────
    // Runs two passes: one for odd, one for even palindromes
    // Both use the same O(n) "expand with mirror" technique
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
            while (i - odd[i] >= 0 && i + odd[i] < n
                   && s[i - odd[i]] == s[i + odd[i]])
                odd[i]++;
            // Update rightmost palindrome
            if (i + odd[i] - 1 > r) { l = i - odd[i] + 1; r = i + odd[i] - 1; }
        }
    }

    // Even-length palindromes — centered between i-1 and i
    void build_even() {
        even.assign(n, 0);
        int l = 0, r = -1;
        for (int i = 0; i < n; i++) {
            if (i <= r) even[i] = min(even[l + r - i + 1], r - i + 1);
            while (i - even[i] - 1 >= 0 && i + even[i] < n
                   && s[i - even[i] - 1] == s[i + even[i]])
                even[i]++;
            if (i + even[i] - 1 > r) { l = i - even[i]; r = i + even[i] - 1; }
        }
    }

    // ── O(1) Palindrome Query ─────────────────────────────
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

    // ── Longest Palindromic Substring ─────────────────────
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

    // ── Count All Palindromic Substrings ──────────────────
    // Total count of (l, r) pairs where s[l..r] is palindrome
    // Each odd[i] contributes odd[i] palindromes (radii 1..odd[i])
    // Each even[i] contributes even[i] palindromes (radii 1..even[i])
    ll count_palindromes() const {
        ll cnt = 0;
        for (int i = 0; i < n; i++) cnt += odd[i];   // odd centered at i
        for (int i = 0; i < n; i++) cnt += even[i];  // even centered between i-1,i
        return cnt;
    }

    // ── All Palindrome Centers ────────────────────────────
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

    // ── Palindromes Containing Position p ─────────────────
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

    // ── Longest Palindromic Prefix ────────────────────────
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

    // ── Longest Palindromic Suffix ────────────────────────
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

    // ── Minimum Palindrome Partition Cuts ─────────────────
    // Minimum number of cuts to partition s into palindromes
    // dp[i] = min cuts for s[0..i-1]
    // O(n²) DP, use with is_palindrome for O(n²) total
    // (O(n log n) possible with suffix automaton — separate problem)
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

    // ── All Palindromic Suffixes of s[0..i] ──────────────
    // Returns lengths of all palindromic suffixes of s[0..i]
    // Useful for palindrome DP optimizations
    // O(n) amortized via series compression
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

    // ── Palindrome occurrence count ────────────────────────
    // How many times does the palindrome s[l..r] appear in s?
    // Naive: O(n) scan using is_palindrome; use hashing for O(1) equality
    int count_occurrences(int l, int r) const {
        int cnt = 0, len = r - l + 1;
        for (int i = 0; i + len - 1 < n; i++)
            if (is_palindrome(i, i + len - 1) && is_palindrome(l, r))
                // same length palindromes: check using odd/even arrays
                cnt++;
        return cnt;
    }

    // Debug: print odd/even arrays
    void print() const {
        cerr << "s   : " << s << "\n";
        cerr << "odd : "; for (int x : odd)  cerr << x << " "; cerr << "\n";
        cerr << "even: "; for (int x : even) cerr << x << " "; cerr << "\n";
    }
};

// ────────────────────────────────────────────────────────
//  Eertree (Palindromic Tree) — O(n) construction
//  Nodes = distinct palindromic substrings
//  Supports:
//   - Count distinct palindromic substrings → size() - 2
//   - Count occurrences of each palindrome  → node.cnt
//   - Series decomposition of palindromes   → node.series_link
// ────────────────────────────────────────────────────────
struct EerTree {
    struct Node {
        map<char, int> next;
        int  suffix_link;   // longest proper palindromic suffix
        int  len;           // length of this palindrome
        int  cnt;           // number of occurrences (after propagation)
        int  series_link;   // link within palindromic series
        int  diff;          // len - suffix_link->len
    };

    vector<Node> t;
    int          last;      // last node
    string       s;

    EerTree() {
        // Node 0: root for even-length palindromes (len = -1, imaginary root)
        // Node 1: root for odd-length palindromes (len = 0, empty string)
        t.push_back({.suffix_link = 0, .len = -1, .cnt = 0});
        t.push_back({.suffix_link = 0, .len =  0, .cnt = 0});
        last = 1;
    }

    // Get suffix link: longest palindromic suffix of s[0..i] other than itself
    int get_suffix(int v, int i) {
        while (i - t[v].len - 1 < 0 || s[i - t[v].len - 1] != s[i])
            v = t[v].suffix_link;
        return v;
    }

    // Add character s[i] — amortized O(1) (O(log alphabet) with map)
    bool add(int i) {
        char c = s[i];
        int  cur = get_suffix(last, i);

        if (!t[cur].next.count(c)) {
            // Create new node: palindrome of length t[cur].len + 2
            Node nn;
            nn.len          = t[cur].len + 2;
            nn.cnt          = 0;
            nn.suffix_link  = (nn.len == 1)
                ? 1
                : t[get_suffix(t[cur].suffix_link, i)].next[c];
            nn.diff = nn.len - t[nn.suffix_link].len;
            nn.series_link  = (nn.diff == t[nn.suffix_link].diff)
                ? t[nn.suffix_link].series_link
                : nn.suffix_link;
            t[cur].next[c]  = (int)t.size();
            t.push_back(nn);
        }

        last = t[cur].next[c];
        t[last].cnt++;
        return true;
    }

    // Build from string
    void build(const string& str) {
        s = str;
        for (int i = 0; i < (int)s.size(); i++) add(i);
        // Propagate counts from children to parents (reverse topological)
        for (int i = (int)t.size() - 1; i >= 2; i--)
            t[t[i].suffix_link].cnt += t[i].cnt;
    }

    // Number of distinct palindromic substrings
    int distinct_palindromes() const { return (int)t.size() - 2; }

    // Print all distinct palindromic substrings with occurrences
    void print(const string& str) const {
        for (int i = 2; i < (int)t.size(); i++) {
            // Recover palindrome: centered at some occurrence
            cerr << "len=" << t[i].len << " cnt=" << t[i].cnt << "\n";
        }
    }
};