#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
ll rand_ll() { return uniform_int_distribution<ll>(0, 1e18)(rng); }

// ═══════════════════════════════════════════════════════
//  Implicit Treap (Sequence Treap)
//
//  Key = implicit position in sequence (not stored)
//  Maintains array with O(log n) operations:
//
//  Supports:
//   - Point set/get                  → set(pos, val) / get(pos)
//   - Range sum/min/max query        → query(l, r)
//   - Range assign/add (lazy)        → assign(l, r, v) / add(l, r, v)
//   - Range reverse                  → reverse(l, r)
//   - Insert at position             → insert(pos, val)
//   - Delete at position             → erase(pos)
//   - Split at position              → split(pos) → (left, right)
//   - Merge two treaps               → merge(left, right)
//   - Rotate subarray [l,r] by k     → rotate(l, r, k)
//   - Move subarray to front/back    → move(l, r, pos)
//   - Sorted insert (order stats)    → insert_sorted(val)
//   - Kth element                    → kth(k)
// ═══════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────
//  Node pool — avoids heap fragmentation
//  MAXN = max nodes ever alive simultaneously
// ────────────────────────────────────────────────────────
const int MAXN = 500005;
const ll  NEG_INF = -1e18;
const ll  POS_INF =  1e18;

struct Node {
    ll   val;           // value at this position
    ll   sum;           // subtree sum
    ll   mn;            // subtree min
    ll   mx;            // subtree max
    ll   lazy_add;      // pending add
    ll   lazy_assign;   // pending assign (NEG_INF = none)
    bool rev;           // pending reverse
    int  sz;            // subtree size
    ll   pri;           // random priority (heap property: parent.pri > child.pri)
    int  left, right;   // child indices (0 = null)
} pool[MAXN];

int pool_ptr = 1;       // 0 = null sentinel
int new_node(ll val) {
    int idx = pool_ptr++;
    pool[idx] = {val, val, val, val, 0, NEG_INF, false, 1, rand_ll(), 0, 0};
    return idx;
}
void reset_pool() { pool_ptr = 1; }

// ── Aggregate helpers ─────────────────────────────────────
ll nd_sum(int t) { return t ? pool[t].sum : 0; }
ll nd_min(int t) { return t ? pool[t].mn  : POS_INF; }
ll nd_max(int t) { return t ? pool[t].mx  : NEG_INF; }
int nd_sz (int t) { return t ? pool[t].sz  : 0; }

void pull(int t) {
    if (!t) return;
    auto& n = pool[t];
    int l = n.left, r = n.right;
    n.sz  = 1 + nd_sz(l) + nd_sz(r);
    n.sum = n.val + nd_sum(l) + nd_sum(r);
    n.mn  = min({n.val, nd_min(l), nd_min(r)});
    n.mx  = max({n.val, nd_max(l), nd_max(r)});
}

// ── Apply a lazy assign to a node ─────────────────────────
void apply_assign(int t, ll v) {
    if (!t) return;
    pool[t].val = pool[t].mn = pool[t].mx = v;
    pool[t].sum = v * pool[t].sz;
    pool[t].lazy_assign = v;
    pool[t].lazy_add = 0;
}

// ── Apply a lazy add to a node ────────────────────────────
void apply_add(int t, ll v) {
    if (!t) return;
    pool[t].val += v;
    pool[t].mn  += v;
    pool[t].mx  += v;
    pool[t].sum += v * pool[t].sz;
    if (pool[t].lazy_assign != NEG_INF)
        pool[t].lazy_assign += v;
    else
        pool[t].lazy_add += v;
}

// ── Apply a reverse to a node ─────────────────────────────
void apply_rev(int t) {
    if (!t) return;
    swap(pool[t].left, pool[t].right);
    pool[t].rev ^= 1;
}

// ── Push lazy to children ─────────────────────────────────
void push(int t) {
    if (!t) return;
    auto& n = pool[t];
    if (n.rev) {
        apply_rev(n.left);
        apply_rev(n.right);
        n.rev = false;
    }
    if (n.lazy_assign != NEG_INF) {
        apply_assign(n.left,  n.lazy_assign);
        apply_assign(n.right, n.lazy_assign);
        n.lazy_assign = NEG_INF;
    }
    if (n.lazy_add) {
        apply_add(n.left,  n.lazy_add);
        apply_add(n.right, n.lazy_add);
        n.lazy_add = 0;
    }
}

// ── Split: first k positions → left, rest → right ─────────
void split(int t, int k, int& l, int& r) {
    if (!t) { l = r = 0; return; }
    push(t);
    int ls = nd_sz(pool[t].left);
    if (ls >= k) {
        split(pool[t].left, k, l, pool[t].left);
        r = t;
    } else {
        split(pool[t].right, k - ls - 1, pool[t].right, r);
        l = t;
    }
    pull(t);
}

// ── Merge: l comes before r in the sequence ───────────────
int merge(int l, int r) {
    if (!l || !r) return l + r;
    push(l); push(r);
    if (pool[l].pri > pool[r].pri) {
        pool[l].right = merge(pool[l].right, r);
        pull(l); return l;
    } else {
        pool[r].left = merge(l, pool[r].left);
        pull(r); return r;
    }
}

// ────────────────────────────────────────────────────────
//  Implicit Treap struct — manages a root and exposes
//  all high-level operations
// ────────────────────────────────────────────────────────
struct ImplicitTreap {
    int root = 0;

    // ── Size ─────────────────────────────────────────────
    int size() { return nd_sz(root); }

    // ── Isolate range [l, r] (0-indexed) ─────────────────
    // Returns {left_part, mid_part, right_part}
    // MUST call reassemble after use
    struct Parts { int l, m, r; };
    Parts isolate(int l, int r) {
        int t1, t2, t3;
        split(root, l, t1, t2);
        split(t2, r - l + 1, t2, t3);
        return {t1, t2, t3};
    }
    void reassemble(Parts p) {
        root = merge(merge(p.l, p.m), p.r);
    }

    // ── Insert value at 0-indexed position ───────────────
    void insert(int pos, ll val) {
        int t1, t2;
        split(root, pos, t1, t2);
        root = merge(merge(t1, new_node(val)), t2);
    }

    // ── Append to end ─────────────────────────────────────
    void push_back(ll val) { root = merge(root, new_node(val)); }
    void push_front(ll val){ root = merge(new_node(val), root); }

    // ── Delete at 0-indexed position ──────────────────────
    void erase(int pos) {
        int t1, t2, t3;
        split(root, pos, t1, t2);
        split(t2, 1, t2, t3);
        root = merge(t1, t3);   // t2 (1 node) is abandoned
    }

    // ── Point get ────────────────────────────────────────
    ll get(int pos) {
        auto [l, m, r] = isolate(pos, pos);
        ll v = pool[m].val;
        reassemble({l, m, r});
        return v;
    }

    // ── Point set ────────────────────────────────────────
    void set(int pos, ll val) {
        auto [l, m, r] = isolate(pos, pos);
        pool[m].val = val; pull(m);
        reassemble({l, m, r});
    }

    // ── Range sum/min/max query ───────────────────────────
    ll query_sum(int l, int r) {
        auto p = isolate(l, r);
        ll v = nd_sum(p.m);
        reassemble(p); return v;
    }
    ll query_min(int l, int r) {
        auto p = isolate(l, r);
        ll v = nd_min(p.m);
        reassemble(p); return v;
    }
    ll query_max(int l, int r) {
        auto p = isolate(l, r);
        ll v = nd_max(p.m);
        reassemble(p); return v;
    }

    // ── Range add ────────────────────────────────────────
    void range_add(int l, int r, ll v) {
        auto p = isolate(l, r);
        apply_add(p.m, v);
        reassemble(p);
    }

    // ── Range assign ──────────────────────────────────────
    void range_assign(int l, int r, ll v) {
        auto p = isolate(l, r);
        apply_assign(p.m, v);
        reassemble(p);
    }

    // ── Range reverse ────────────────────────────────────
    void range_reverse(int l, int r) {
        auto p = isolate(l, r);
        apply_rev(p.m);
        reassemble(p);
    }

    // ── Rotate subarray [l, r] left by k positions ───────
    // e.g. "abcde" rotate [1,3] by 1 → "acdbe" → wait
    // rotate [0,4] by 2 → "cdeab"
    void rotate_left(int l, int r, int k) {
        k %= (r - l + 1);
        if (!k) return;
        auto p = isolate(l, r);
        int t1, t2;
        split(p.m, k, t1, t2);
        p.m = merge(t2, t1);
        reassemble(p);
    }

    // ── Move subarray [l, r] to position pos ─────────────
    // (pos is in the array BEFORE extraction)
    void move_to(int l, int r, int pos) {
        int t1, t2, t3;
        split(root, l, t1, t2);
        split(t2, r - l + 1, t2, t3);
        // Now t2 is the subarray, pos must be adjusted
        int new_pos = pos - l;
        if (new_pos < 0)       new_pos = 0;
        if (new_pos > nd_sz(t1) + nd_sz(t3)) new_pos = nd_sz(t1) + nd_sz(t3);
        int r1, r2;
        int combined = merge(t1, t3);
        split(combined, new_pos, r1, r2);
        root = merge(merge(r1, t2), r2);
    }

    // ── Kth element (1-indexed) ───────────────────────────
    ll kth(int k) {
        int t = root;
        while (t) {
            push(t);
            int ls = nd_sz(pool[t].left);
            if (k == ls + 1) return pool[t].val;
            if (k <= ls) t = pool[t].left;
            else { k -= ls + 1; t = pool[t].right; }
        }
        return NEG_INF;
    }

    // ── Walk in order (inorder traversal) ────────────────
    void walk(int t, vector<ll>& out) {
        if (!t) return;
        push(t);
        walk(pool[t].left, out);
        out.push_back(pool[t].val);
        walk(pool[t].right, out);
    }
    vector<ll> to_array() {
        vector<ll> v; walk(root, v); return v;
    }
};

// ────────────────────────────────────────────────────────
//  Explicit Key Treap (sorted BST / order statistics)
//  Key is stored explicitly; tree is a sorted set
//
//  Supports:
//   - Insert key                     → insert(key)
//   - Delete key                     → erase(key)
//   - Count keys ≤ x                 → order_of(x)
//   - Kth smallest key               → find_kth(k)
//   - Predecessor / Successor        → prev(x) / next(x)
//   - Split by key                   → split_by_key(x)
// ────────────────────────────────────────────────────────
const int MAXN2 = 500005;

struct KeyNode {
    ll  key;
    ll  pri;
    int sz;
    int left, right;
    int cnt;   // multiplicity (for multiset behaviour)
} kpool[MAXN2];

int kpool_ptr = 1;
int new_knode(ll key) {
    int idx = kpool_ptr++;
    kpool[idx] = {key, rand_ll(), 1, 0, 0, 1};
    return idx;
}

int knd_sz(int t) { return t ? kpool[t].sz : 0; }

void kpull(int t) {
    if (!t) return;
    kpool[t].sz = knd_sz(kpool[t].left) + knd_sz(kpool[t].right) + kpool[t].cnt;
}

// Split: keys < x → l, keys ≥ x → r
void ksplit(int t, ll x, int& l, int& r) {
    if (!t) { l = r = 0; return; }
    if (kpool[t].key < x) {
        ksplit(kpool[t].right, x, kpool[t].right, r);
        l = t;
    } else {
        ksplit(kpool[t].left, x, l, kpool[t].left);
        r = t;
    }
    kpull(t);
}

int kmerge(int l, int r) {
    if (!l || !r) return l + r;
    if (kpool[l].pri > kpool[r].pri) {
        kpool[l].right = kmerge(kpool[l].right, r);
        kpull(l); return l;
    } else {
        kpool[r].left = kmerge(l, kpool[r].left);
        kpull(r); return r;
    }
}

struct ExplicitTreap {
    int root = 0;

    void insert(ll key) {
        int l, r;
        ksplit(root, key, l, r);
        int m, r2;
        ksplit(r, key + 1, m, r2);
        if (m) { kpool[m].cnt++; kpull(m); }
        else    m = new_knode(key);
        root = kmerge(kmerge(l, m), r2);
    }

    void erase(ll key) {
        int l, r, m, r2;
        ksplit(root, key, l, r);
        ksplit(r, key + 1, m, r2);
        if (m) {
            if (--kpool[m].cnt == 0) m = kmerge(kpool[m].left, kpool[m].right);
            else kpull(m);
        }
        root = kmerge(kmerge(l, m), r2);
    }

    // Number of elements strictly less than key
    int order_of(ll key) {
        int l, r;
        ksplit(root, key, l, r);
        int res = knd_sz(l);
        root = kmerge(l, r);
        return res;
    }

    // Kth smallest (1-indexed)
    ll find_kth(int k) {
        int t = root;
        while (t) {
            int ls = knd_sz(kpool[t].left);
            if (k <= ls) { t = kpool[t].left; continue; }
            k -= ls;
            if (k <= kpool[t].cnt) return kpool[t].key;
            k -= kpool[t].cnt;
            t = kpool[t].right;
        }
        return NEG_INF;
    }

    bool contains(ll key) {
        int t = root;
        while (t) {
            if (kpool[t].key == key) return kpool[t].cnt > 0;
            t = key < kpool[t].key ? kpool[t].left : kpool[t].right;
        }
        return false;
    }

    // Largest key strictly < x
    ll prev_key(ll x) {
        int l, r;
        ksplit(root, x, l, r);
        ll res = NEG_INF;
        int t = l;
        while (t) { res = kpool[t].key; t = kpool[t].right; }
        root = kmerge(l, r);
        return res;
    }

    // Smallest key strictly > x
    ll next_key(ll x) {
        int l, r;
        ksplit(root, x + 1, l, r);
        ll res = POS_INF;
        int t = r;
        while (t) { res = kpool[t].key; t = kpool[t].left; }
        root = kmerge(l, r);
        return res;
    }

    int size() { return knd_sz(root); }
};