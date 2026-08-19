#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef unsigned long long ull;
typedef pair<ll,ll> pll;

// ═══════════════════════════════════════════════════════
//  String Hashing Template
//
//  Variants:
//   1. Single hash (fast, small collision risk)
//   2. Double hash (recommended, near-zero collision)
//   3. Rolling hash (sliding window)
//   4. 2D hash     (grid/matrix patterns)
//
//  Supports:
//   - Substring hash in O(1)        → get(l, r)
//   - Substring equality in O(1)    → equal(l1,r1,l2,r2)
//   - LCP (binary search)           → lcp(i, j)
//   - Palindrome check              → is_palindrome(l, r)
//   - Pattern matching (Rabin-Karp) → find_all(pattern)
//   - Distinct substrings count     → count_distinct()
//   - Lexicographic comparison      → compare(l1,r1,l2,r2)
// ═══════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────
//  Hash parameters
//  Two independent (base, mod) pairs for double hashing
//  Bases chosen > alphabet size, mods chosen as large primes
//  Using random bases at runtime prevents anti-hash attacks
// ────────────────────────────────────────────────────────
struct HashParams {
    ll base, mod;
};

// Fixed params (fast but hackable in adversarial problems)
const HashParams P1 = {131,   1000000007LL};
const HashParams P2 = {137,   998244353LL };

// Random base (use in competitive programming to prevent hack)
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
ll random_base(ll lo = 256, ll hi = 1e9) {
    return uniform_int_distribution<ll>(lo, hi)(rng);
}

// ────────────────────────────────────────────────────────
//  1. Single Polynomial Hash
//  h(s) = s[0]*B^(n-1) + s[1]*B^(n-2) + ... + s[n-1]*B^0 (mod M)
//  get(l,r) = hash of s[l..r] in O(1)
// ────────────────────────────────────────────────────────
struct SingleHash {
    int     n;
    ll      base, mod;
    vector<ll> h, pw;   // prefix hashes and powers

    SingleHash() {}
    SingleHash(const string& s, ll base, ll mod)
        : n(s.size()), base(base), mod(mod), h(n+1, 0), pw(n+1, 1) {

        for (int i = 0; i < n; i++) {
            h[i+1]  = (h[i] * base + s[i]) % mod;
            pw[i+1] = pw[i] * base % mod;
        }
    }

    // hash of s[l..r] inclusive, 0-indexed
    ll get(int l, int r) const {
        return (h[r+1] - h[l] * pw[r-l+1] % mod + mod * 2) % mod;
    }

    // Concatenate: hash of s[l1..r1] + s[l2..r2]
    ll concat(int l1, int r1, int l2, int r2) const {
        int len2 = r2 - l2 + 1;
        return (get(l1, r1) * pw[len2] + get(l2, r2)) % mod;
    }
};

// ────────────────────────────────────────────────────────
//  2. Double Hash (recommended — collision probability ~1/MOD^2)
//  Stores (hash1, hash2) as a pair for O(1) equality
// ────────────────────────────────────────────────────────
struct DoubleHash {
    int n;
    SingleHash h1, h2;

    DoubleHash() {}
    DoubleHash(const string& s,
               HashParams p1 = P1, HashParams p2 = P2)
        : n(s.size()),
          h1(s, p1.base, p1.mod),
          h2(s, p2.base, p2.mod) {}

    // Returns combined hash of s[l..r]
    pll get(int l, int r) const {
        return {h1.get(l, r), h2.get(l, r)};
    }

    // s[l1..r1] == s[l2..r2]?
    bool equal(int l1, int r1, int l2, int r2) const {
        return get(l1, r1) == get(l2, r2);
    }

    // Longest Common Prefix of s[i..] and s[j..]
    int lcp(int i, int j) const {
        int lo = 0, hi = min(n - i, n - j);
        while (lo < hi) {
            int mid = lo + (hi - lo + 1) / 2;
            if (equal(i, i+mid-1, j, j+mid-1)) lo = mid;
            else                                hi = mid - 1;
        }
        return lo;
    }

    // Is s[l..r] a palindrome?
    bool is_palindrome(int l, int r) const {
        // Build reverse hash separately — see DoubleHashPalin below
        // Placeholder: use two-pointer or Manacher for production use
        int len = r - l + 1;
        for (int i = 0; i < len / 2; i++)
            if (get(l+i, l+i) != get(r-i, r-i)) return false;
        return true;   // O(n) — replace with reverse hash for O(1)
    }

    // Lexicographic comparison of s[l1..r1] vs s[l2..r2]
    // Returns -1, 0, +1
    int compare(int l1, int r1, int l2, int r2,
                const string& s) const {
        int len1 = r1 - l1 + 1, len2 = r2 - l2 + 1;
        int common_lcp = 0;
        int lo = 0, hi = min(len1, len2);
        while (lo < hi) {
            int mid = lo + (hi - lo + 1) / 2;
            if (equal(l1, l1+mid-1, l2, l2+mid-1)) lo = mid;
            else                                     hi = mid - 1;
        }
        common_lcp = lo;

        if (common_lcp == len1 && common_lcp == len2) return 0;
        if (common_lcp == len1) return -1;
        if (common_lcp == len2) return +1;
        return s[l1 + common_lcp] < s[l2 + common_lcp] ? -1 : +1;
    }
};

// ────────────────────────────────────────────────────────
//  3. Palindrome-aware Double Hash
//  Builds BOTH forward and reverse hashes
//  Enables O(1) palindrome check for any substring
// ────────────────────────────────────────────────────────
struct PalinHash {
    DoubleHash fwd, rev;
    int n;

    PalinHash(const string& s) : n(s.size()), fwd(s) {
        string r(s.rbegin(), s.rend());
        rev = DoubleHash(r);
    }

    // O(1) palindrome check: s[l..r]
    bool is_palindrome(int l, int r) const {
        int rev_l = n - 1 - r;
        int rev_r = n - 1 - l;
        return fwd.get(l, r) == rev.get(rev_l, rev_r);
    }

    pll get(int l, int r) const { return fwd.get(l, r); }
    bool equal(int l1, int r1, int l2, int r2) const {
        return fwd.equal(l1, r1, l2, r2);
    }
    int lcp(int i, int j) const { return fwd.lcp(i, j); }
};

// ────────────────────────────────────────────────────────
//  4. Rolling Hash (sliding window, fixed window size k)
//  Useful for: find all anagrams, sliding window distinct,
//              Rabin-Karp with window of size k
// ────────────────────────────────────────────────────────
struct RollingHash {
    int     k;
    ll      base, mod;
    ll      cur_hash, highest_pw;   // pw = base^(k-1) mod mod

    RollingHash(const string& s, int k, ll base, ll mod)
        : k(k), base(base), mod(mod), cur_hash(0) {

        highest_pw = 1;
        for (int i = 0; i < k - 1; i++) highest_pw = highest_pw * base % mod;

        // Compute initial window hash
        for (int i = 0; i < k; i++)
            cur_hash = (cur_hash * base + s[i]) % mod;
    }

    // Slide window: remove char_out (leftmost), add char_in (new rightmost)
    void slide(char char_out, char char_in) {
        cur_hash = (cur_hash - (ll)char_out * highest_pw % mod + mod) % mod;
        cur_hash = (cur_hash * base + char_in) % mod;
    }

    ll get() const { return cur_hash; }
};

// ────────────────────────────────────────────────────────
//  5. Multi-string Hash (for suffix array, string set)
//  Enables hashing multiple strings with shared power table
// ────────────────────────────────────────────────────────
struct MultiHash {
    ll      base, mod;
    vector<ll> pw;   // shared power table, size = max length

    void precompute(int max_len) {
        pw.resize(max_len + 1);
        pw[0] = 1;
        for (int i = 1; i <= max_len; i++)
            pw[i] = pw[i-1] * base % mod;
    }

    MultiHash(ll base, ll mod, int max_len)
        : base(base), mod(mod) {
        precompute(max_len);
    }

    // Returns prefix hash array for string s
    vector<ll> build(const string& s) {
        int n = s.size();
        vector<ll> h(n + 1, 0);
        for (int i = 0; i < n; i++)
            h[i+1] = (h[i] * base + s[i]) % mod;
        return h;
    }

    // Query substring [l,r] given prefix hash array
    ll get(const vector<ll>& h, int l, int r) {
        int len = r - l + 1;
        return (h[r+1] - h[l] * pw[len] % mod + mod * 2) % mod;
    }
};

// ────────────────────────────────────────────────────────
//  6. 2D Hash (grid/matrix pattern matching)
//  h[i][j] = hash of rectangle [0..i][0..j]
//  get(r1,c1,r2,c2) = hash of subgrid [r1..r2][c1..c2]
// ────────────────────────────────────────────────────────
struct Hash2D {
    int n, m;
    ll base_r, base_c, mod;
    vector<vector<ll>> h;
    vector<ll> pw_r, pw_c;

    Hash2D(const vector<string>& grid,
           ll base_r = 131, ll base_c = 137, ll mod = 1e9+7)
        : n(grid.size()), m(grid[0].size()),
          base_r(base_r), base_c(base_c), mod(mod),
          h(n+1, vector<ll>(m+1, 0)),
          pw_r(n+1, 1), pw_c(m+1, 1) {

        for (int i = 1; i <= n; i++) pw_r[i] = pw_r[i-1] * base_r % mod;
        for (int j = 1; j <= m; j++) pw_c[j] = pw_c[j-1] * base_c % mod;

        for (int i = 1; i <= n; i++)
            for (int j = 1; j <= m; j++)
                h[i][j] = (h[i-1][j] * base_r
                          + h[i][j-1] * base_c
                          - h[i-1][j-1] * base_r % mod * base_c % mod
                          + grid[i-1][j-1]
                          + mod * 4) % mod;
    }

    // hash of subgrid [r1..r2][c1..c2] — 0-indexed inclusive
    ll get(int r1, int c1, int r2, int c2) {
        int rows = r2 - r1 + 1, cols = c2 - c1 + 1;
        ll res = h[r2+1][c2+1]
               - h[r1][c2+1] * pw_r[rows] % mod
               - h[r2+1][c1] * pw_c[cols] % mod
               + h[r1][c1]   * pw_r[rows] % mod * pw_c[cols] % mod;
        return (res % mod + mod * 4) % mod;
    }

    bool equal(int r1, int c1, int r2, int c2,
               int r3, int c3, int r4, int c4) {
        if (r2-r1 != r4-r3 || c2-c1 != c4-c3) return false;
        return get(r1,c1,r2,c2) == get(r3,c3,r4,c4);
    }
};

// ────────────────────────────────────────────────────────
//  Application: Rabin-Karp Pattern Matching
//  Find all occurrences of pattern in text — O(n+m)
// ────────────────────────────────────────────────────────
vector<int> rabin_karp(const string& text, const string& pattern,
                        HashParams p = P1) {
    int n = text.size(), m = pattern.size();
    if (m > n) return {};

    SingleHash ht(text,    p.base, p.mod);
    SingleHash hp(pattern, p.base, p.mod);
    ll pat_hash = hp.get(0, m - 1);

    vector<int> result;
    for (int i = 0; i + m - 1 < n; i++) {
        if (ht.get(i, i + m - 1) == pat_hash)
            result.push_back(i);
    }
    return result;
}

// ────────────────────────────────────────────────────────
//  Application: Count Distinct Substrings
//  O(n² log n) using hashing — O(n log²n) with SA
// ────────────────────────────────────────────────────────
ll count_distinct_substrings(const string& s) {
    int n = s.size();
    DoubleHash dh(s);
    set<pll> seen;
    for (int l = 0; l < n; l++)
        for (int r = l; r < n; r++)
            seen.insert(dh.get(l, r));
    return (ll)seen.size();
}

// ────────────────────────────────────────────────────────
//  Application: Longest Common Substring of two strings
//  Binary search on length + hash set — O(n log n)
// ────────────────────────────────────────────────────────
int longest_common_substring(const string& a, const string& b) {
    int lo = 0, hi = min((int)a.size(), (int)b.size());
    DoubleHash ha(a), hb(b);

    auto check = [&](int len) -> bool {
        unordered_set<ll> seen;
        for (int i = 0; i + len <= (int)a.size(); i++)
            seen.insert(ha.get(i, i+len-1).first);
        for (int j = 0; j + len <= (int)b.size(); j++)
            if (seen.count(hb.get(j, j+len-1).first)) return true;
        return false;
    };

    while (lo < hi) {
        int mid = lo + (hi - lo + 1) / 2;
        if (check(mid)) lo = mid;
        else            hi = mid - 1;
    }
    return lo;
}

// ────────────────────────────────────────────────────────
//  Application: Minimum Period of a String
//  Smallest p such that s[i] = s[i mod p] for all i
// ────────────────────────────────────────────────────────
int min_period(const string& s) {
    int n = s.size();
    DoubleHash dh(s);
    for (int p = 1; p <= n; p++) {
        if (n % p != 0) continue;
        bool ok = true;
        for (int i = p; i < n && ok; i += p)
            if (!dh.equal(0, p-1, i, i+p-1)) ok = false;
        if (ok) return p;
    }
    return n;
}