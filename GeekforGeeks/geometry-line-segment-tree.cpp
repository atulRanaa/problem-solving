#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll INF = 2e18;

// ═══════════════════════════════════════════════════════
//  Li Chao Tree (Segment Tree of Lines)
//
//  Core idea:
//   - Each node stores the "dominant" line at its midpoint
//   - Adding a line: compare with current dominant at mid,
//     keep winner at node, push loser to whichever child
//     it might dominate in
//   - Query min/max at x: walk root→leaf, take best along path
//
//  Two implementations:
//   1. Static  — fixed coordinate range [L, R], array-based
//   2. Dynamic — arbitrary x values, pointer/pool-based
//
//  Supports:
//   - Add line y = m*x + b              → add_line(m, b)
//   - Add line segment [xl, xr]         → add_segment(m, b, xl, xr)
//   - Query min/max at x                → query(x)
//   - Convex hull trick problems        → add_line + query
//   - DP optimization (1D/1D)           → see usage section
// ═══════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────
//  1. Static Li Chao Tree (fixed integer range [L, R])
//  O(log(R-L)) per add_line and query
//  Memory: O((R-L) * 2) nodes
//  Use when: coordinate range is known and not too large
// ────────────────────────────────────────────────────────
template <typename T, bool MINIMIZE = true>
struct LiChaoStatic {
private:
    struct Line {
        T m, b;
        T eval(T x) const { return m * x + b; }
    };

    struct Node {
        Line line;
        int  left, right;   // children indices, 0 = null
        bool has_line;
    };
    
    vector<Node> pool;
    int root;
    T L, R;

    // For MINIMIZE: +INF line. For MAXIMIZE: -INF line.
    static constexpr T WORST = MINIMIZE ? (T)2e18 : (T)(-2e18);

    static bool better(T a, T b) {
        return MINIMIZE ? a < b : a > b;
    }

    int new_node() {
        pool.push_back({{0, WORST}, 0, 0, false});
        return pool.size() - 1;
    }

    void add_line_impl(int node, T l, T r, Line line) {
        if (!pool[node].has_line) {
            pool[node].line     = line;
            pool[node].has_line = true;
            return;
        }
        T mid = l + (r - l) / 2;
        bool left_better  = better(line.eval(l),   pool[node].line.eval(l));
        bool mid_better   = better(line.eval(mid),  pool[node].line.eval(mid));

        if (mid_better) swap(pool[node].line, line);   // new line dominates at mid

        if (l == r) return;

        if (left_better != mid_better) {
            if (!pool[node].left) pool[node].left = new_node();
            add_line_impl(pool[node].left, l, mid, line);
        } else {
            if (!pool[node].right) pool[node].right = new_node();
            add_line_impl(pool[node].right, mid + 1, r, line);
        }
    }

    void add_segment_impl(int node, T l, T r, Line line, T xl, T xr) {
        if (xr < l || r < xl) return;         // no overlap
        if (xl <= l && r <= xr) {             // full overlap
            add_line_impl(node, l, r, line);
            return;
        }
        T mid = l + (r - l) / 2;
        if (!pool[node].left)  pool[node].left  = new_node();
        if (!pool[node].right) pool[node].right  = new_node();
        add_segment_impl(pool[node].left,  l,     mid, line, xl, xr);
        add_segment_impl(pool[node].right, mid+1, r,   line, xl, xr);
    }

    T query_impl(int node, T l, T r, T x) const {
        if (!node) return WORST;
        T res = pool[node].has_line ? pool[node].line.eval(x) : WORST;
        if (l == r) return res;
        T mid = l + (r - l) / 2;
        T child_res;
        if (x <= mid)
            child_res = query_impl(pool[node].left,  l,     mid, x);
        else
            child_res = query_impl(pool[node].right, mid+1, r,   x);
        return better(child_res, res) ? child_res : res;
    }

public:
    explicit LiChaoStatic(T L, T R) : L(L), R(R) {
        pool.push_back({{0, WORST}, 0, 0, false});  // pool[0] = null sentinel
        root = new_node();
    }

    // add line y = m*x + b to entire range
    void add_line(T m, T b) {
        add_line_impl(root, L, R, {m, b});
    }

    // add line segment y = m*x + b for x in [xl, xr]
    void add_segment(T m, T b, T xl, T xr) {
        add_segment_impl(root, L, R, {m, b}, xl, xr);
    }

    T query(T x) const {
        return query_impl(root, L, R, x);
    }
};

// ────────────────────────────────────────────────────────
//  2. Dynamic Li Chao Tree (arbitrary / large x values)
//  Use when: x coordinates are large (up to 1e18) but few
//  Memory: O(q log(R-L)) — only creates nodes on demand
// ────────────────────────────────────────────────────────
template <typename T, bool MINIMIZE = true>
struct LiChaoDynamic {
    struct Line {
        T m, b;
        T eval(T x) const { return m * x + b; }
    };

    static constexpr T WORST = MINIMIZE ? (T)2e18 : (T)(-2e18);
    static bool better(T a, T b) { return MINIMIZE ? a < b : a > b; }

    struct Node {
        Line line;
        int  left = 0, right = 0;
        bool has_line = false;
    };

    vector<Node> pool;
    int          root;
    T            L, R;

    int new_node() {
        pool.push_back({});
        return pool.size() - 1;
    }

    LiChaoDynamic(T L, T R) : L(L), R(R) {
        pool.push_back({});   // pool[0] = null sentinel
        root = new_node();
    }

    void add_line(T m, T b) {
        add_line_impl(root, L, R, {m, b});
    }

    void add_line_impl(int node, T l, T r, Line line) {
        if (!pool[node].has_line) {
            pool[node].line     = line;
            pool[node].has_line = true;
            return;
        }
        T mid = l + (r - l) / 2;
        bool left_better = better(line.eval(l),   pool[node].line.eval(l));
        bool mid_better  = better(line.eval(mid),  pool[node].line.eval(mid));

        if (mid_better) swap(pool[node].line, line);
        if (l == r) return;

        if (left_better != mid_better) {
            if (!pool[node].left) pool[node].left = new_node();
            add_line_impl(pool[node].left, l, mid, line);
        } else {
            if (!pool[node].right) pool[node].right = new_node();
            add_line_impl(pool[node].right, mid+1, r, line);
        }
    }

    void add_segment(T m, T b, T xl, T xr) {
        add_segment_impl(root, L, R, {m, b}, xl, xr);
    }

    void add_segment_impl(int node, T l, T r, Line line, T xl, T xr) {
        if (xr < l || r < xl) return;
        if (xl <= l && r <= xr) { add_line_impl(node, l, r, line); return; }
        T mid = l + (r - l) / 2;
        if (!pool[node].left)  pool[node].left  = new_node();
        if (!pool[node].right) pool[node].right = new_node();
        add_segment_impl(pool[node].left,  l,     mid, line, xl, xr);
        add_segment_impl(pool[node].right, mid+1, r,   line, xl, xr);
    }

    T query(T x) const { return query_impl(root, L, R, x); }

    T query_impl(int node, T l, T r, T x) const {
        if (!node) return WORST;
        T res = pool[node].has_line ? pool[node].line.eval(x) : WORST;
        if (l == r) return res;
        T mid = l + (r - l) / 2;
        T child_res = (x <= mid)
            ? query_impl(pool[node].left,  l,     mid, x)
            : query_impl(pool[node].right, mid+1, r,   x);
        return better(child_res, res) ? child_res : res;
    }
};

// ────────────────────────────────────────────────────────
//  3. Convex Hull Trick (CHT) — O(n) amortized
//  Use ONLY when queries/lines are monotone (sorted slopes)
//  Much faster than Li Chao for monotone inputs
//  Included here for comparison / fallback
// ────────────────────────────────────────────────────────
template <bool MINIMIZE = true>
struct ConvexHullTrick {
    struct Line { ll m, b; ll eval(ll x) { return m*x + b; } };
    deque<Line> hull;

    static bool bad(Line l1, Line l2, Line l3) {
        // l2 is never optimal if intersection of l1,l3 is left of l1,l2
        return (__int128)(l3.b - l1.b) * (l1.m - l2.m)
             <= (__int128)(l2.b - l1.b) * (l1.m - l3.m);
    }

    // Add line with DECREASING slope (for minimum) or INCREASING (for maximum)
    void add(ll m, ll b) {
        Line l = {m, b};
        if (MINIMIZE) {
            while (hull.size() >= 2 && bad(hull[hull.size()-2], hull[hull.size()-1], l))
                hull.pop_back();
        } else {
            while (hull.size() >= 2 && bad(hull[hull.size()-2], hull[hull.size()-1], l))
                hull.pop_back();
        }
        hull.push_back(l);
    }

    // Query with INCREASING x (pointer moves forward only)
    ll query_monotone(ll x) {
        while (hull.size() > 1) {
            bool worse = MINIMIZE
                ? hull[0].eval(x) >= hull[1].eval(x)
                : hull[0].eval(x) <= hull[1].eval(x);
            if (worse) hull.pop_front();
            else break;
        }
        return hull[0].eval(x);
    }

    // Query arbitrary x (binary search) — O(log n)
    ll query(ll x) {
        int lo = 0, hi = hull.size() - 1;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            bool worse = MINIMIZE
                ? hull[mid].eval(x) >= hull[mid+1].eval(x)
                : hull[mid].eval(x) <= hull[mid+1].eval(x);
            if (worse) lo = mid + 1;
            else       hi = mid;
        }
        return hull[lo].eval(x);
    }
};

// Convenience aliases
using LiChaoMin = LiChaoStatic<ll, true>;
using LiChaoMax = LiChaoStatic<ll, false>;
using LiChaoDynMin = LiChaoDynamic<ll, true>;
using LiChaoDynMax = LiChaoDynamic<ll, false>;
using CHTMin = ConvexHullTrick<true>;
using CHTMax = ConvexHullTrick<false>;