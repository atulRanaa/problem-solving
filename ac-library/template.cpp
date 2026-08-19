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



struct SuffixAutomaton {
private:
    struct State {
        map<char,int> next;
        int link, len;
        ll  cnt; // occurrences

        int firstpos;
    };

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
    
    int find_firstpos(const string& pat) const {
        int cur = 0;
        for (char c : pat) {
            auto it = st[cur].next.find(c);
            if (it == st[cur].next.end()) return -1;
            cur = it->second;
        }
        return st[cur].firstpos - (int)pat.size() + 1;
    }

    pair<int, int> solve() {
        // longest repeating string
        int len = 0, pos;
        for(auto& s: st) {
            if(s.cnt > 1 && s.len > len) {
                pos = s.firstpos;
                len = s.len;
            }
        }

        return {pos - len + 1, len};
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

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
const ll M = 991831889;
const ll C = uniform_int_distribution<ll>(0.1 * M, 0.9 * M)(rng);

struct HashString {
    int n;
    vector<ll> pows, sums;

    HashString(string s) : n(s.size()), pows(n + 1, 1), sums(n + 1) {
        for (int i = 1; i <= n; i++) {
            pows[i] = pows[i - 1] * C % M;
            sums[i] = (sums[i - 1] * C + s[i - 1]) % M;
        }
    }

    // Returns the hash of the substring [l, r)
    ll hash(int l, int r) {
        ll h = sums[r] - sums[l] * pows[r - l];
        return (h % M + M) % M;
    }
};



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






// ═══════════════════════════════════════════════════════
//  Matrix Binary Exponentiation Template
//
//  Supports:
//   - Matrix multiply          → operator*
//   - Matrix power             → mat_pow(M, k)
//   - Identity matrix          → identity(n)
//   - Apply to vector          → apply(M, v)
//   - Fibonacci (O(log n))     → fibonacci(n)
//   - Linear recurrence        → linear_recurrence(coeffs, init, n)
//   - Path counting in graphs  → mat_pow(adj, k)
//   - System of recurrences    → custom transition matrix
// ═══════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────
//  Core Matrix struct
//  Templated on type T and modulus — works for ll, int, double
// ────────────────────────────────────────────────────────
template <typename T, T MOD_VAL = 0>
struct Matrix {
private:
    int n, m;
    vector<vector<T>> a;

public:
    // Constructors
    Matrix() : n(0), m(0) {}
    Matrix(int n, int m, T val = 0) : n(n), m(m), a(n, vector<T>(m, val)) {}
    Matrix(vector<vector<T>> v) : n(v.size()), m(v[0].size()), a(v) {}

    // Access
    vector<T>&       operator[](int i)       { return a[i]; }
    const vector<T>& operator[](int i) const { return a[i]; }

    // Identity matrix (n×n)
    static Matrix identity(int n) {
        Matrix I(n, n, 0);
        for (int i = 0; i < n; i++) I[i][i] = 1;
        return I;
    }

    // Zero matrix
    static Matrix zero(int n, int m) { return Matrix(n, m, 0); }

    Matrix operator*(const Matrix& B) const {
        assert(m == B.n);
        Matrix C(n, B.m, 0);
        for (int i = 0; i < n; i++)
            for (int k = 0; k < m; k++) {
                if (a[i][k] == 0) continue;   // skip zero rows (optimization)
                for (int j = 0; j < B.m; j++) {
                    C[i][j] += a[i][k] * B[k][j];
                    if constexpr (MOD_VAL != 0)
                        C[i][j] %= MOD_VAL;
                }
            }
        return C;
    }

    Matrix operator+(const Matrix& B) const {
        assert(n == B.n && m == B.m);
        Matrix C(n, m);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++) {
                C[i][j] = a[i][j] + B[i][j];
                if constexpr (MOD_VAL != 0)
                    C[i][j] %= MOD_VAL;
            }
        return C;
    }

    Matrix operator*(T scalar) const {
        Matrix C(n, m);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++) {
                C[i][j] = a[i][j] * scalar;
                if constexpr (MOD_VAL != 0)
                    C[i][j] %= MOD_VAL;
            }
        return C;
    }

    bool operator==(const Matrix& B) const { return a == B.a; }

    vector<T> apply(const vector<T>& v) const {
        assert(m == (int)v.size());
        vector<T> res(n, 0);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++) {
                res[i] += a[i][j] * v[j];
                if constexpr (MOD_VAL != 0)
                    res[i] %= MOD_VAL;
            }
        return res;
    }

    Matrix pow(ll k) const {
        assert(n == m);   // must be square
        Matrix result = identity(n);
        Matrix base   = *this;
        while (k > 0) {
            if (k & 1) result = result * base;
            base = base * base;
            k >>= 1;
        }
        return result;
    }

    void print() const {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++)
                cerr << a[i][j] << " \n"[j == m-1];
        }
    }
};

using Mat = Matrix<ll, MOD>;







// ═══════════════════════════════════════════════════════
//  Heavy-Light Decomposition
//
//  Supports:
//   - Path query     (node weights)    → query_path(u, v)
//   - Path update    (node weights)    → update_path(u, v, val)
//   - Subtree query                    → query_subtree(u)
//   - Subtree update                   → update_subtree(u, val)
//   - Path query     (edge weights)    → query_path_edge(u, v)
//   - Path update    (edge weights)    → update_path_edge(u, v, val)
//   - LCA                              → lca(u, v)
//
//  Works with ANY segment tree exposing:
//    seg.prod(l, r)       → range query  [l, r) half-open
//    seg.apply(l, r, val) → range update [l, r) half-open
//    seg.set(pos, val)    → point set
//
//  Build:
//    HLD hld(n);
//    for each edge: hld.add_edge(u, v);
//    hld.init(root);
//    build segment tree on hld.order[] (flattened node values)
// ═══════════════════════════════════════════════════════
struct HLD {
private:
    int n, timer;
    vector<vector<int>> adj;
    vector<int> parent, depth, sz, heavy, head, pos, pos_end, order;
    // pos[u]     = start position of u in flattened array
    // pos_end[u] = end position of subtree of u (exclusive)
    // order[i]   = which node maps to flattened index i

    explicit HLD(int n)
        : n(n), timer(0),
          adj(n + 1), parent(n + 1, 0), depth(n + 1, 0),
          sz(n + 1, 0), heavy(n + 1, -1), head(n + 1, 0),
          pos(n + 1, 0), pos_end(n + 1, 0), order(n + 1, 0) {}

    void add_edge(int u, int v) { adj[u].push_back(v); adj[v].push_back(u); }

    // compute subtree sizes, identify heavy children
    void dfs_size(int root) {
        stack<pair<int,int>> stk;
        stk.push({root, 0});
        vector<int> order_stk;

        // Forward pass: set parent/depth, record traversal order
        while (!stk.empty()) {
            auto [u, p] = stk.top(); stk.pop();
            parent[u] = p;
            sz[u] = 1;
            order_stk.push_back(u);
            for (int v : adj[u]) {
                if (v == p) continue;
                depth[v] = depth[u] + 1;
                stk.push({v, u});
            }
        }

        // Reverse pass: compute sizes + heavy children bottom-up
        for (int i = (int)order_stk.size() - 1; i >= 0; i--) {
            int u = order_stk[i];
            int best = 0;
            for (int v : adj[u]) {
                if (v == parent[u]) continue;
                sz[u] += sz[v];
                if (sz[v] > best) { best = sz[v]; heavy[u] = v; }
            }
        }
    }

    // assign HLD positions
    void dfs_decompose(int root) {
        // Stack stores {node, chain_head}
        // Push light children with their own head, heavy child with same head
        stack<pair<int,int>> stk;
        stk.push({root, root});

        while (!stk.empty()) {
            auto [u, h] = stk.top(); stk.pop();
            head[u] = h;
            pos[u]  = timer;
            order[timer] = u;
            timer++;

            // Push light children first (they'll be processed last = after heavy chain)
            for (int v : adj[u]) {
                if (v == parent[u] || v == heavy[u]) continue;
                
                // light → new chain starting at v
                stk.push({v, v});
            }
            // Push heavy child last → it pops first → chain stays contiguous
            if (heavy[u] != -1) stk.push({heavy[u], h});
        }

        // Subtree of u = [pos[u], pos[u] + sz[u])
        for (int u = 1; u <= n; u++)
            pos_end[u] = pos[u] + sz[u];
    }

    // collect chain segments between u and v
    // Returns list of [l, r) half-open intervals in flattened array
    // For edge queries: exclude the LCA node (top node of last segment)
    vector<pair<int,int>> path_segments(int u, int v, bool edge_query = false) {
        vector<pair<int,int>> segs;
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) 
                swap(u, v);
            segs.push_back({pos[head[u]], pos[u] + 1});   // [pos[head[u]], pos[u])
            u = parent[head[u]];
        }
        if (depth[u] > depth[v]) swap(u, v);
        // For edge queries: exclude LCA itself (its edge is to parent, not in path)
        int lo = edge_query ? pos[u] + 1 : pos[u];
        segs.push_back({lo, pos[v] + 1});
        return segs;
    }

public:
    void init(int root = 1) {
        timer = 0;
        dfs_size(root);
        dfs_decompose(root);
    }

    int lca(int u, int v) {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            u = parent[head[u]];
        }
        return depth[u] < depth[v] ? u : v;
    }

    // Path query: op must be commutative across segments (e.g. sum, min, max)
    template <typename SegTree>
    auto query_path(int u, int v, SegTree& seg) {
        auto res = seg.all_prod() ^ seg.all_prod();  // type-deduced zero/identity
        // Note: ^ is wrong for identity; use explicit identity instead — see usage below
        for (auto [l, r] : path_segments(u, v))
            res = res + seg.prod(l, r);   // customize combiner for your op
        return res;
    }

    // Better: pass combiner and identity explicitly
    template <typename SegTree, typename T, typename Combine>
    T query_path(int u, int v, SegTree& seg, T identity, Combine combine) {
        T res = identity;
        for (auto [l, r] : path_segments(u, v))
            res = combine(res, seg.prod(l, r));
        return res;
    }

    template <typename SegTree, typename T, typename Combine>
    T query_path_edge(int u, int v, SegTree& seg, T identity, Combine combine) {
        T res = identity;
        for (auto [l, r] : path_segments(u, v, true))
            res = combine(res, seg.prod(l, r));
        return res;
    }

    // Path update (range apply)
    template <typename SegTree, typename F>
    void update_path(int u, int v, SegTree& seg, F f) {
        for (auto [l, r] : path_segments(u, v))
            seg.apply(l, r, f);
    }

    template <typename SegTree, typename F>
    void update_path_edge(int u, int v, SegTree& seg, F f) {
        for (auto [l, r] : path_segments(u, v, true))
            seg.apply(l, r, f);
    }

    // Subtree query — [pos[u], pos[u] + sz[u]) is the entire subtree
    template <typename SegTree>
    auto query_subtree(int u, SegTree& seg) {
        return seg.prod(pos[u], pos_end[u]);
    }

    // Subtree update
    template <typename SegTree, typename F>
    void update_subtree(int u, SegTree& seg, F f) {
        seg.apply(pos[u], pos_end[u], f);
    }

    // Point set
    template <typename SegTree, typename S>
    void update_node(int u, SegTree& seg, S val) {
        seg.set(pos[u], val);
    }
};




// ═══════════════════════════════════════════════════════
//  Persistent Segment Tree
//  Template params match ACL segtree exactly:
//    S    — node value type
//    op   — associative combine: (S, S) → S
//    e    — identity: () → S  (op(e(), x) == x)
//
//  Supports:
//   - Build from array                 → persistent_segtree(v)
//   - Point set (new version)          → set(ver, idx, val)
//   - Point add (new version)          → add(ver, idx, diff)
//   - Range query on any version       → prod(ver, l, r)
//   - Single point get                 → get(ver, idx)
//   - Kth order statistic (freq trees) → kth(ver_l, ver_r, k)
//   - Reset pool between test cases    → reset_pool()
//
//  MAXNODES = (n + q) × log2(n) × 2   (tune per problem)
//  Each set/add call costs O(log n) new nodes
// ═══════════════════════════════════════════════════════

template <class S, S (*op)(S, S), S (*e)()>
struct persistent_segtree {
private:

    // ── Memory pool ──────────────────────────────────────
    struct Node {
        S   val;
        int left, right;   // child indices; 0 = null
    };

    static constexpr int MAXNODES = 20000000;
    inline static Node pool[MAXNODES];
    inline static int  pool_ptr = 1;   // 0 reserved as null/identity node

    // ── Instance state ────────────────────────────────────
    int         _n;
    vector<int> roots;     // roots[i] = root node index of version i

    // ── Internal helpers ──────────────────────────────────

    int new_node(S val, int lc = 0, int rc = 0) {
        assert(pool_ptr < MAXNODES);
        pool[pool_ptr] = {val, lc, rc};
        return pool_ptr++;
    }

    // Build a full tree from array [l, r] — O(n)
    int build(const vector<S>& v, int l, int r) {
        if (l == r)
            return new_node(l < (int)v.size() ? v[l] : e());
        int mid = l + (r - l) / 2;
        int lc  = build(v, l,     mid);
        int rc  = build(v, mid+1, r  );
        return new_node(op(pool[lc].val, pool[rc].val), lc, rc);
    }

    // Path-copying point SET — creates O(log n) new nodes
    int update_set(int prev, int l, int r, int idx, S val) {
        if (l == r) return new_node(val);
        int mid = l + (r - l) / 2;
        int lc  = pool[prev].left;
        int rc  = pool[prev].right;
        if (idx <= mid) lc = update_set(lc, l,     mid, idx, val);
        else            rc = update_set(rc, mid+1, r,   idx, val);
        return new_node(op(pool[lc].val, pool[rc].val), lc, rc);
    }

    // Path-copying point ADD (op applied on top) — for frequency trees
    int update_add(int prev, int l, int r, int idx, S diff) {
        if (l == r) return new_node(op(pool[prev].val, diff));
        int mid = l + (r - l) / 2;
        int lc  = pool[prev].left;
        int rc  = pool[prev].right;
        if (idx <= mid) lc = update_add(lc, l,     mid, idx, diff);
        else            rc = update_add(rc, mid+1, r,   idx, diff);
        return new_node(op(pool[lc].val, pool[rc].val), lc, rc);
    }

    // Range query on a single version
    S query(int node, int l, int r, int ql, int qr) {
        if (!node || ql > r || qr < l) return e();
        if (ql <= l && r <= qr) return pool[node].val;
        int mid = l + (r - l) / 2;
        return op(query(pool[node].left,  l,     mid, ql, qr),
                  query(pool[node].right, mid+1, r,   ql, qr));
    }

    // Kth order statistic via two-version subtraction
    // Precondition: S is numeric, op = addition, e() = 0
    // ln/rn are root indices of two versions; k is 1-indexed
    int kth_impl(int ln, int rn, int l, int r, ll k) {
        if (l == r) return l;
        int mid    = l + (r - l) / 2;
        ll left_cnt = (ll)pool[pool[rn].left].val
                    - (ll)pool[pool[ln].left].val;
        if (k <= left_cnt) {
            return kth_impl(pool[ln].left,  pool[rn].left,  l,     mid, k);
        } else {
            return kth_impl(pool[ln].right, pool[rn].right, mid+1, r,   k - left_cnt);
        }
    }

public:

    // ── Constructors ──────────────────────────────────────

    // Empty tree of size n (all identity)
    explicit persistent_segtree(int n) : _n(n) {
        vector<S> v(n, e());
        roots.push_back(build(v, 0, n - 1));
    }

    // Build from existing array (version 0)
    explicit persistent_segtree(const vector<S>& v) : _n((int)v.size()) {
        roots.push_back(build(v, 0, _n - 1));
    }

    // ── Point operations (each returns new version id) ────

    // Point set: version[ver] with position idx changed to val
    int set(int ver, int idx, S val) {
        assert(0 <= ver && ver < (int)roots.size());
        assert(0 <= idx && idx < _n);
        roots.push_back(update_set(roots[ver], 0, _n - 1, idx, val));
        return (int)roots.size() - 1;
    }

    // Point add: version[ver] with op(arr[idx], diff) at idx
    // Primary use: frequency trees (diff = 1 to insert element)
    int add(int ver, int idx, S diff) {
        assert(0 <= ver && ver < (int)roots.size());
        assert(0 <= idx && idx < _n);
        roots.push_back(update_add(roots[ver], 0, _n - 1, idx, diff));
        return (int)roots.size() - 1;
    }

    // ── Queries ───────────────────────────────────────────

    // Single point get on version ver (0-indexed)
    S get(int ver, int idx) {
        assert(0 <= ver && ver < (int)roots.size());
        return query(roots[ver], 0, _n - 1, idx, idx);
    }

    // Range query [l, r] on version ver (0-indexed, inclusive)
    S prod(int ver, int l, int r) {
        assert(0 <= ver && ver < (int)roots.size());
        assert(0 <= l && l <= r && r < _n);
        return query(roots[ver], 0, _n - 1, l, r);
    }

    // All elements query on version ver
    S all_prod(int ver) {
        assert(0 <= ver && ver < (int)roots.size());
        return pool[roots[ver]].val;
    }

    // Kth smallest among elements inserted between version ver_l and ver_r
    // ver_l is exclusive lower bound (typically ver_l = build ver = 0)
    // ver_r is inclusive upper bound
    // k is 1-indexed
    // Returns: coordinate index into the seg tree range [0, n)
    //          → decompress with your sorted array externally
    // PRECONDITION: S = int/ll, op = add, e() = 0
    int kth(int ver_l, int ver_r, ll k) {
        assert(0 <= ver_l && ver_r < (int)roots.size());
        return kth_impl(roots[ver_l], roots[ver_r], 0, _n - 1, k);
    }

    // ── Utility ───────────────────────────────────────────

    int latest()       const { return (int)roots.size() - 1; }
    int num_versions() const { return (int)roots.size(); }

    // Reset shared pool between test cases
    static void reset_pool() {
        pool_ptr = 1;
        pool[0]  = {e(), 0, 0};   // ensure null node holds identity
    }
};


// ═══════════════════════════════════════════════════════
//  Max Flow: Dinic's Algorithm  O(V² E)
//  Bipartite Matching:          O(E √V)
//  Min Cut:                     falls out of max flow
//  Min Cost Max Flow (MCMF):    O(VE log V · flow)
// ═══════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────
//  1. Dinic's Max Flow
// ────────────────────────────────────────────────────────
struct Dinic {
    struct Edge {
        int to, rev;
        ll  cap;
    };

    int n;
    vector<vector<Edge>> graph;
    vector<int> level, iter;

    explicit Dinic(int n) : n(n), graph(n), level(n), iter(n) {}

    // add directed edge u→v with capacity cap
    // automatically adds reverse edge with cap 0
    void add_edge(int u, int v, ll cap) {
        graph[u].push_back({v, (int)graph[v].size(),     cap});
        graph[v].push_back({u, (int)graph[u].size() - 1, 0  });
    }

    // add undirected edge (both directions with full cap)
    void add_edge_undirected(int u, int v, ll cap) {
        graph[u].push_back({v, (int)graph[v].size(),     cap});
        graph[v].push_back({u, (int)graph[u].size() - 1, cap});
    }

    // BFS to build level graph from source s
    bool bfs(int s, int t) {
        fill(level.begin(), level.end(), -1);
        queue<int> Q;
        level[s] = 0;
        Q.push(s);
        while (!Q.empty()) {
            int v = Q.front(); 
            Q.pop();
            
            for (auto& e: graph[v]) {
                if (e.cap > 0 && level[e.to] < 0) {
                    level[e.to] = level[v] + 1;
                    Q.push(e.to);
                }
            }
        }
        return level[t] >= 0;
    }

    // DFS to push flow along level graph
    ll dfs(int v, int t, ll f) {
        if (v == t) return f;
        for (int& i = iter[v]; i < (int)graph[v].size(); i++) {
            Edge& e = graph[v][i];
            if (e.cap > 0 && level[v] < level[e.to]) {
                ll d = dfs(e.to, t, min(f, e.cap));
                if (d > 0) {
                    e.cap -= d;
                    graph[e.to][e.rev].cap += d;
                    return d;
                }
            }
        }
        return 0;
    }

    // Returns maximum flow from s to t
    ll max_flow(int s, int t) {
        ll flow = 0;
        while (bfs(s, t)) {
            fill(iter.begin(), iter.end(), 0);
            ll d;
            while ((d = dfs(s, t, INF)) > 0) flow += d;
        }
        return flow;
    }

    // Min cut: call AFTER max_flow()
    // Returns {cut value, set of nodes reachable from s in residual}
    pair<ll, vector<bool>> min_cut(int s, int t) {
        ll flow = max_flow(s, t);
        vector<bool> visited(n, false);
        queue<int> Q;
        Q.push(s); visited[s] = true;
        while (!Q.empty()) {
            int v = Q.front(); Q.pop();
            for (auto& e : graph[v])
                if (e.cap > 0 && !visited[e.to]) {
                    visited[e.to] = true;
                    Q.push(e.to);
                }
        }
        return {flow, visited};
        // cut edges: u→v where visited[u]=true, visited[v]=false
    }

    // Recover flow on each original edge
    // Call after max_flow(); pass original capacity to compare
    vector<pair<int,ll>> get_flow_on_edges(vector<tuple<int,int,ll>>& original_edges) {
        vector<pair<int,ll>> result;
        int idx = 0;
        for (auto& [u, v, cap] : original_edges) {
            ll residual = graph[u][idx].cap;
            result.push_back({idx, cap - residual});
            idx++;
        }
        return result;
    }
};

// ────────────────────────────────────────────────────────
//  2. Bipartite Matching (via Dinic)
//  Left nodes:  1..L
//  Right nodes: L+1..L+R
//  Source: 0,  Sink: L+R+1
// ────────────────────────────────────────────────────────
struct BipartiteMatching {
    int L, R, S, T;
    Dinic dinic;

    BipartiteMatching(int L, int R)
        : L(L), R(R), S(0), T(L + R + 1), dinic(L + R + 2) {
        for (int i = 1; i <= L; i++) dinic.add_edge(S, i, 1);
        for (int j = 1; j <= R; j++) dinic.add_edge(L + j, T, 1);
    }

    // Add edge between left node u (1-indexed) and right node v (1-indexed)
    void add_edge(int u, int v) { dinic.add_edge(u, L + v, 1); }

    ll max_matching() { return dinic.max_flow(S, T); }

    // Returns matching pairs {left, right} (1-indexed each side)
    vector<pair<int,int>> get_matching() {
        max_matching();
        vector<pair<int,int>> res;
        for (int u = 1; u <= L; u++)
            for (auto& e : dinic.graph[u])
                if (e.to != S && e.cap == 0)  // saturated edge → matched
                    res.push_back({u, e.to - L});
        return res;
    }

    // Minimum vertex cover (Konig's theorem)
    // Returns {left nodes in cover, right nodes in cover}
    pair<vector<int>,vector<int>> min_vertex_cover() {
        max_matching();
        auto [_, reachable] = dinic.min_cut(S, T);

        vector<int> left_cover, right_cover;
        for (int u = 1; u <= L; u++)
            if (!reachable[u]) left_cover.push_back(u);    // NOT reachable from S
        for (int v = 1; v <= R; v++)
            if (reachable[L + v]) right_cover.push_back(v); // reachable from S
        return {left_cover, right_cover};
    }
};

// ────────────────────────────────────────────────────────
//  3. Min Cost Max Flow (MCMF)  using SPFA (Bellman-Ford BFS)
//  Use when you need: cheapest way to push max flow
//  O(V · E · flow) — replace SPFA with Dijkstra+potentials
//  (Johnson's trick) for better performance on large graphs
// ────────────────────────────────────────────────────────
struct MCMF {
    struct Edge {
        int to, rev;
        ll cap, cost;
    };

    int n;
    vector<vector<Edge>> graph;

    explicit MCMF(int n) : n(n), graph(n) {}

    void add_edge(int u, int v, ll cap, ll cost) {
        graph[u].push_back({v, (int)graph[v].size(),      cap,  cost});
        graph[v].push_back({u, (int)graph[u].size() - 1,  0,   -cost});
    }

    // Returns {total_flow, total_cost}
    pair<ll,ll> min_cost_flow(int s, int t, ll max_f = INF) {
        ll flow = 0, cost = 0;
        while (flow < max_f) {
            // SPFA: find shortest (cheapest) augmenting path
            vector<ll>  dist(n, INF);
            vector<bool> in_queue(n, false);
            vector<int>  prev_v(n, -1), prev_e(n, -1);
            queue<int> Q;
            dist[s] = 0; in_queue[s] = true; Q.push(s);
            while (!Q.empty()) {
                int v = Q.front(); Q.pop(); in_queue[v] = false;
                for (int i = 0; i < (int)graph[v].size(); i++) {
                    auto& e = graph[v][i];
                    if (e.cap > 0 && dist[v] + e.cost < dist[e.to]) {
                        dist[e.to] = dist[v] + e.cost;
                        prev_v[e.to] = v; prev_e[e.to] = i;
                        if (!in_queue[e.to]) {
                            in_queue[e.to] = true;
                            Q.push(e.to);
                        }
                    }
                }
            }
            if (dist[t] == INF) break;  // no augmenting path

            // find bottleneck capacity along path
            ll d = max_f - flow;
            for (int v = t; v != s; v = prev_v[v])
                d = min(d, graph[prev_v[v]][prev_e[v]].cap);

            // augment
            for (int v = t; v != s; v = prev_v[v]) {
                auto& e = graph[prev_v[v]][prev_e[v]];
                e.cap -= d;
                graph[v][e.rev].cap += d;
            }
            flow += d; cost += d * dist[t];
        }
        return {flow, cost};
    }
};


struct scc_graph {
  public:
    int n;
    int num_scc;
    std::vector<std::vector<int>> adj;
    std::vector<std::vector<int>> radj;
    std::vector<int> comp; // Maps original node -> SCC ID

    scc_graph() : n(0), num_scc(0) {}
    scc_graph(int n) : n(n), num_scc(0), adj(n), radj(n), comp(n, -1) {}

    // Add a directed edge from u to v (0-indexed)
    void add_edge(int u, int v) {
        adj[u].push_back(v);
        radj[v].push_back(u);
    }

    // Computes the Strongly Connected Components
    void build() {
        std::vector<bool> vis(n, false);
        std::vector<int> order;
        order.reserve(n);

        for (int i = 0; i < n; i++) {
            if (!vis[i]) {
                dfs1(i, vis, order);
            }
        }

        std::fill(comp.begin(), comp.end(), -1);
        num_scc = 0;

        for (int i = n - 1; i >= 0; i--) {
            int root = order[i];
            if (comp[root] == -1) {
                dfs2(root, num_scc++);
            }
        }
    }

    // Condenses the SCCs into a Directed Acyclic Graph (DAG)
    // Returns an adjacency list of unique component IDs
    std::vector<std::vector<int>> condensation() {
        std::vector<std::vector<int>> dag(num_scc);
        // Track edges already inserted to avoid duplicate neighbors efficiently
        std::vector<int> last_seen(num_scc, -1);

        for (int u = 0; u < n; u++) {
            int u_comp = comp[u];
            for (int v : adj[u]) {
                int v_comp = comp[v];
                if (u_comp != v_comp && last_seen[v_comp] != u_comp) {
                    dag[u_comp].push_back(v_comp);
                    last_seen[v_comp] = u_comp; // Prevent multi-edge duplicates
                }
            }
        }
        return dag;
    }

    // Returns groups of original vertices inside each component
    std::vector<std::vector<int>> groups() {
        std::vector<int> group_sizes(num_scc, 0);
        for (int i = 0; i < n; i++) {
            group_sizes[comp[i]]++;
        }
        std::vector<std::vector<int>> res(num_scc);
        for (int i = 0; i < num_scc; i++) {
            res[i].reserve(group_sizes[i]);
        }
        for (int i = 0; i < n; i++) {
            res[comp[i]].push_back(i);
        }
        return res;
    }

  private:
    void dfs1(int node, std::vector<bool>& vis, std::vector<int>& order) {
        vis[node] = true;
        for (int v : adj[node]) {
            if (!vis[v]) dfs1(v, vis, order);
        }
        order.push_back(node);
    }

    void dfs2(int node, int id) {
        comp[node] = id;
        for (int v : radj[node]) {
            if (comp[v] == -1) dfs2(v, id);
        }
    }
};

template <typename T = int>
struct dsu {
  public:
    dsu() : _n(0) {}
    dsu(T n) : _n(n), parent_or_size(n, -1) {}

    T merge(T a, T b) {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        T x = leader(a), y = leader(b);
        if (x == y) return x;
        if (-parent_or_size[x] < -parent_or_size[y]) std::swap(x, y);
        parent_or_size[x] += parent_or_size[y];
        parent_or_size[y] = x;
        return x;
    }

    bool same(T a, T b) {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        return leader(a) == leader(b);
    }

    T leader(T a) {
        assert(0 <= a && a < _n);
        if (parent_or_size[a] < 0) return a;
        return parent_or_size[a] = leader(parent_or_size[a]);
    }

    T size(T a) {
        assert(0 <= a && a < _n);
        return -parent_or_size[leader(a)];
    }

    std::vector<std::vector<T>> groups() {
        std::vector<T> leader_buf(_n), group_size(_n, 0);
        for (T i = 0; i < _n; i++) {
            leader_buf[i] = leader(i);
            group_size[leader_buf[i]]++;
        }
        std::vector<std::vector<T>> result(_n);
        for (T i = 0; i < _n; i++) {
            result[i].reserve(group_size[i]);
        }
        for (T i = 0; i < _n; i++) {
            result[leader_buf[i]].push_back(i);
        }
        result.erase(
            std::remove_if(result.begin(), result.end(),
                           [&](const std::vector<T>& v) { return v.empty(); }),
            result.end());
        return result;
    }

  private:
    T _n;
    // root node: -1 * component size
    // otherwise: parent
    std::vector<T> parent_or_size;
};



// ═══════════════════════════════════════════════════════
//  Generic Directed Graph Template
//
//  Covers:
//   - Cycle detection                   → has_cycle()
//   - Topological sort                  → topo_sort()
//   - Longest / Shortest path in DAG    → dag_dp()
//   - Reachability (multi-source BFS)   → reachable()
//   - Strongly Connected Components     → scc() [Kosaraju]
// ═══════════════════════════════════════════════════════

struct DirectedGraph {
    int n;
    vector<vector<int>>  adj;   // original edges
    vector<vector<int>> radj;   // reversed edges (for SCC / backward reachability)

    // ── Internal DFS state ──────────────────────────────
    vector<int> color;          // 0=white, 1=gray(on stack), 2=black(done)
    vector<int> parent;         // parent in DFS tree, -1 = no parent
    vector<int> topology;       // reverse-finished order (topological when reversed)
    bool        has_cycle;
    int         cycle_start, cycle_end;

    // ── SCC state ───────────────────────────────────────
    vector<int> comp;           // comp[v] = SCC id of node v
    int         num_scc;

    explicit DirectedGraph(int n)
        : n(n), adj(n + 1), radj(n + 1),
          color(n + 1, 0), parent(n + 1, -1), has_cycle(false),
          cycle_start(-1), cycle_end(-1),
          comp(n + 1, -1), num_scc(0) {}

    void add_edge(int u, int v) {
        adj[u].push_back(v);
        radj[v].push_back(u);
    }

    // ────────────────────────────────────────────────────
    //  1. DFS: cycle detection + topological sort
    //     After calling build_topology(), check has_cycle.
    //     If !has_cycle, reverse topology to get topo order.
    // ────────────────────────────────────────────────────
    void dfs(int node) {
        color[node] = 1;        // gray: on current DFS stack
        for (int v : adj[node]) {
            if (has_cycle) return;
            if (color[v] == 1) {            // back edge → cycle
                has_cycle  = true;
                cycle_start = v;
                cycle_end   = node;
                return;
            }
            if (color[v] == 0) {            // tree edge → recurse
                parent[v] = node;
                dfs(v);
            }
            // color[v] == 2: cross/forward edge → skip
        }
        color[node] = 2;                    // black: fully processed
        topology.push_back(node);           // post-order
    }

    // Run DFS from ALL nodes (handles disconnected graphs)
    void build_topology() {
        fill(color.begin(),  color.end(),  0);
        fill(parent.begin(), parent.end(), -1);
        topology.clear();
        has_cycle   = false;
        cycle_start = cycle_end = -1;

        for (int i = 1; i <= n; i++) {
            if (color[i] == 0) {
                dfs(i);
                if (has_cycle) return;
            }
        }
        reverse(topology.begin(), topology.end()); // now in topological order
    }

    // ────────────────────────────────────────────────────
    //  2. Cycle reconstruction
    //     Call after build_topology() when has_cycle == true
    // ────────────────────────────────────────────────────
    vector<int> get_cycle() {
        // walk parent[] from cycle_end up to cycle_start
        vector<int> cycle;
        for (int v = cycle_end; v != cycle_start; v = parent[v])
            cycle.push_back(v);
        cycle.push_back(cycle_start);
        reverse(cycle.begin(), cycle.end());
        cycle.push_back(cycle_start);    // close the cycle
        return cycle;
    }

    // ────────────────────────────────────────────────────
    //  3. DAG DP: longest / shortest path from src to dst
    //     Must call build_topology() first and confirm !has_cycle
    //
    //     mode: +1 = longest path (init -inf)
    //           -1 = shortest path (init +inf)
    // ────────────────────────────────────────────────────
    struct DPResult {
        vector<ll>  dist;
        vector<int> par;
        bool        reachable;
    };

    DPResult dag_dp(int src, int dst, int mode = 1,
                    vector<ll> edge_w = {}) {
        // default: unit weights if no edge weights provided
        bool unit = edge_w.empty();

        const ll INIT = (mode == 1) ? -1e18 : 1e18;
        vector<ll>  dist(n + 1, INIT);
        vector<int> par(n + 1, -1);
        dist[src] = 0;

        for (int node : topology) {
            if (dist[node] == INIT) continue;   // unreachable from src
            for (int i = 0; i < (int)adj[node].size(); i++) {
                int  v = adj[node][i];
                ll   w = unit ? 1 : edge_w[i];
                ll   nd = dist[node] + w;
                bool better = (mode == 1) ? nd > dist[v] : nd < dist[v];
                if (better) {
                    dist[v] = nd;
                    par[v]  = node;
                }
            }
        }

        return {dist, par, dist[dst] != INIT};
    }

    // Reconstruct path from dag_dp result
    vector<int> get_path(const DPResult& res, int src, int dst) {
        if (!res.reachable) return {};
        vector<int> path;
        for (int v = dst; v != -1; v = res.par[v])
            path.push_back(v);
        reverse(path.begin(), path.end());
        return path;
    }

    // ────────────────────────────────────────────────────
    //  4. Multi-source BFS reachability
    //     Returns boolean array: reach[v] = can any source reach v?
    //     Pass use_radj=true to find "who can reach dst" (backward)
    // ────────────────────────────────────────────────────
    vector<bool> reachable(vector<int> sources, bool use_radj = false) {
        auto& g = use_radj ? radj : adj;
        vector<bool> reach(n + 1, false);
        queue<int> Q;
        for (int s : sources) {
            if (!reach[s]) { reach[s] = true; Q.push(s); }
        }
        while (!Q.empty()) {
            int node = Q.front(); Q.pop();
            for (int v : g[node]) {
                if (!reach[v]) { reach[v] = true; Q.push(v); }
            }
        }
        return reach;
    }

    // ────────────────────────────────────────────────────
    //  5. Strongly Connected Components (Kosaraju's)
    //     After calling scc():
    //       comp[v]  = SCC id of node v (0-indexed)
    //       num_scc  = total number of SCCs
    //       SCCs are numbered in reverse topological order
    //       (comp[v]==0 is the "last" SCC in topo order)
    // ────────────────────────────────────────────────────
    void scc_dfs1(int node, vector<bool>& vis, vector<int>& order) {
        vis[node] = true;
        for (int v : adj[node])
            if (!vis[v]) scc_dfs1(v, vis, order);
        order.push_back(node);
    }

    void scc_dfs2(int node, int id) {
        comp[node] = id;
        for (int v : radj[node])
            if (comp[v] == -1) scc_dfs2(v, id);
    }

    void scc() {
        vector<bool> vis(n + 1, false);
        vector<int>  order;
        for (int i = 1; i <= n; i++)
            if (!vis[i]) scc_dfs1(i, vis, order);

        fill(comp.begin(), comp.end(), -1);
        num_scc = 0;
        for (int i = (int)order.size() - 1; i >= 0; i--)
            if (comp[order[i]] == -1)
                scc_dfs2(order[i], num_scc++);
    }

    // Condense into DAG of SCCs (useful after scc())
    // Returns adjacency list on SCC ids
    vector<set<int>> condensation() {
        vector<set<int>> dag(num_scc);
        for (int u = 1; u <= n; u++)
            for (int v : adj[u])
                if (comp[u] != comp[v])
                    dag[comp[u]].insert(comp[v]);
        return dag;
    }
};



å// ────────────────────────────────────────────────
//  Generic Bipartite / 2-Coloring Template
//
//  Solves:
//   - Is the graph bipartite?                 → solve()
//   - Team/partition assignment               → get_teams()
//   - Odd cycle detection                     → !is_bipartite
//   - Connected component coloring            → color[]
//   - Foundation for bipartite matching       → adj + color[]
// ────────────────────────────────────────────────
struct BipartiteGraph {
    int n;
    vector<vector<int>> adj;
    vector<int> color;       // color[i] = 0 or 1 (team), -1 = unvisited
    vector<int> comp;        // comp[i]  = which connected component node i belongs to
    bool is_bipartite;
    int num_components;

    explicit BipartiteGraph(int n)
        : n(n), adj(n), color(n, -1), comp(n, -1),
          is_bipartite(true), num_components(0) {}

    void add_edge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // BFS 2-color a single component starting from src
    // Returns false if an odd cycle is found
    bool bfs(int src, int id) {
        queue<int> Q;
        Q.push(src);
        color[src] = 0;
        comp[src]  = id;

        while (!Q.empty()) {
            int node = Q.front();
            Q.pop();

            for (int next : adj[node]) {
                if (color[next] == -1) {
                    color[next] = color[node] ^ 1;   // flip color
                    comp[next]  = id;
                    Q.push(next);
                } else if (color[next] == color[node]) {
                    return false;    // same color on both ends → odd cycle
                }
                // color[next] == color[node]^1 → already correctly colored, skip
            }
        }
        return true;
    }

    // Run over all nodes to handle disconnected graphs
    bool solve() {
        for (int i = 0; i < n; i++) {
            if (color[i] == -1) {
                if (!bfs(i, num_components++)) {
                    is_bipartite = false;
                    return false;
                }
            }
        }
        return true;
    }

    // Returns {team_0_nodes, team_1_nodes}
    // Only meaningful if is_bipartite == true
    pair<vector<int>, vector<int>> get_teams() {
        vector<int> t0, t1;
        for (int i = 0; i < n; i++) {
            (color[i] == 0 ? t0 : t1).push_back(i);
        }
        return {t0, t1};
    }

    // Nodes in a specific component
    vector<int> get_component(int id) {
        vector<int> nodes;
        for (int i = 0; i < n; i++)
            if (comp[i] == id) nodes.push_back(i);
        return nodes;
    }
};
// BipartiteGraph G(n + 1);


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

template <typename T>
struct pbds_tree {
    tree<
        pair<T, int>, 
        null_type, 
        less<pair<T, int>>, 
        rb_tree_tag, 
        tree_order_statistics_node_update
    > s;
    int timer = 0; // Incremental unique tracker ID

    void insert(T val) {
        s.insert({val, ++timer});
    }

    bool erase(T val) {
        auto it = s.lower_bound({val, 0});
        if (it != s.end() && it->first == val) {
            s.erase(it);
            return true;
        }
        return false;
    }

    // Count numbers strictly less than val
    int count_less_than(T val) {
        return s.order_of_key({val, 0});
    }

    // Count numbers less than or equal to val
    int count_less_equal(T val) {
        // Using a high marker ensures we sit past all elements matching 'val'
        return s.order_of_key({val, 2e9}); 
    }

    // Find the k-th smallest element (0-indexed)
    T find_kth(int k) {
        if (k < 0 || k >= (int)s.size()) return -1; // Out of bounds check
        return s.find_by_order(k)->first;
    }

    int size() { return s.size(); }
};

// Fenwick Tree (BIT) 0-indexed interface: add(p, x) adds x at position p; sum(l, r) = sum of [l, r)
template <class T>
struct fenwick_tree {
    using U = typename std::make_unsigned<T>::type;

  public:
    fenwick_tree() : _n(0) {}
    explicit fenwick_tree(int n) : _n(n), data(n) {}

    void add(int p, T x) {
        assert(0 <= p && p < _n);
        p++;
        while (p <= _n) {
            data[p - 1] += U(x);
            p += p & -p;
        }
    }

    // sum of [0, r)
    T sum(int r) {
        assert(0 <= r && r <= _n);
        U s = 0;
        while (r > 0) {
            s += data[r - 1];
            r -= r & -r;
        }
        return T(s);
    }

    // sum of [l, r)
    T sum(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        return sum(r) - sum(l);
    }

    int kth(T k) {
        assert(_n > 0 && 1 <= k && k <= sum(_n));
        int pos = 0;
        // Start from highest power of 2 <= _n
        for (int pw = 1 << (31 - __builtin_clz(_n)); pw > 0; pw >>= 1) {
            if (pos + pw <= _n && data[pos + pw - 1] < U(k)) {
                pos += pw;
                k -= T(data[pos - 1]);
            }
        }
        return pos;  // 0-indexed result
    }
    
  private:
    int _n;
    std::vector<U> data;
};


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
        ll res = (sum(r) - sum(l)) % mod;
        if (res < 0) res += mod;
        return res;
    }

  private:
    int _n;
    ll mod;
    vector<ll> data;
};


template <class S, S (*op)(S, S), S (*e)()>
struct segtree {
  public:
    segtree() : segtree(0) {}
    segtree(int n) : segtree(std::vector<S>(n, e())) {}
    segtree(const std::vector<S>& v) : _n(int(v.size())) {
        log = 0;
        while ((1 << log) < _n) log++;
        size = 1 << log;
        d = std::vector<S>(2 * size, e());
        for (int i = 0; i < _n; i++) d[size + i] = v[i];
        for (int i = size - 1; i >= 1; i--) update(i);
    }

    void set(int p, S x) {
        assert(0 <= p && p < _n);
        p += size;
        d[p] = x;
        for (int i = 1; i <= log; i++) update(p >> i);
    }

    S get(int p) {
        assert(0 <= p && p < _n);
        return d[p + size];
    }

    S prod(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        S sml = e(), smr = e();
        l += size;
        r += size;
        while (l < r) {
            if (l & 1) sml = op(sml, d[l++]);
            if (r & 1) smr = op(d[--r], smr);
            l >>= 1;
            r >>= 1;
        }
        return op(sml, smr);
    }

    S all_prod() { return d[1]; }

    template <class F> int max_right(int l, F f) {
        assert(0 <= l && l <= _n);
        assert(f(e()));
        if (l == _n) return _n;
        l += size;
        S sm = e();
        do {
            while (l % 2 == 0) l >>= 1;
            if (!f(op(sm, d[l]))) {
                while (l < size) {
                    l = 2 * l;
                    if (f(op(sm, d[l]))) { sm = op(sm, d[l]); l++; }
                }
                return l - size;
            }
            sm = op(sm, d[l]);
            l++;
        } while ((l & -l) != l);
        return _n;
    }

    template <class F> int min_left(int r, F f) {
        assert(0 <= r && r <= _n);
        assert(f(e()));
        if (r == 0) return 0;
        r += size;
        S sm = e();
        do {
            r--;
            while (r > 1 && (r % 2)) r >>= 1;
            if (!f(op(d[r], sm))) {
                while (r < size) {
                    r = 2 * r + 1;
                    if (f(op(d[r], sm))) { sm = op(d[r], sm); r--; }
                }
                return r + 1 - size;
            }
            sm = op(d[r], sm);
        } while ((r & -r) != r);
        return 0;
    }

  private:
    int _n, size, log;
    std::vector<S> d;
    void update(int k) { d[k] = op(d[2 * k], d[2 * k + 1]); }
};

// Range Xor
ll op_xor(ll a, ll b) { return a ^ b; }
ll e_xor() { return 0; }
using segtree_xor = segtree<ll, op_xor, e_xor>;

// Range Sum
ll op_sum(ll a, ll b) { return a + b; }
ll e_sum() { return 0; }
using segtree_sum = segtree<ll, op_sum, e_sum>;

// Range Min
ll op_min(ll a, ll b) { return min(a, b); }
ll e_min() { return inf; }   // use a large sentinel, e.g. const ll inf = 1e18;
using segtree_min = segtree<ll, op_min, e_min>;

// Range Max
ll op_max(ll a, ll b) { return max(a, b); }
ll e_max() { return -inf; }
using segtree_max = segtree<ll, op_max, e_max>;


struct mergetree {
  public:
    mergetree(const std::vector<int>& v) : _n(int(v.size())) {
        log = 0;
        while ((1 << log) < _n) log++;
        size = 1 << log;
        d = std::vector< vector<int> >(2 * size);
        for (int i = 0; i < _n; i++) d[size + i] = {v[i]};
        for (int i = size - 1; i >= 1; i--) update(i);
    }

    int op(const vector<int>& v, int x, int y) {
        auto right_it = upper_bound(v.begin(), v.end(), y);
        auto left_it = lower_bound(v.begin(), v.end(), x);
        return right_it - left_it;
    }

    int prod(int l, int r, int x, int y) {
        assert(0 <= l && l <= r && r <= _n);
        int ans = 0;
        l += size;
        r += size;
        while (l < r) {
            if (l & 1) ans += op(d[l++], x, y);
            if (r & 1) ans += op(d[--r], x, y);
            l >>= 1;
            r >>= 1;
        }
        return ans;
    }

  private:  
    int _n, size, log;
    vector< vector<int> > d;
    void update(int k) {
        int l = 2 * k, r = 2 * k + 1;

        d[k].resize(d[l].size() + d[r].size());
        std::merge(d[l].begin(), d[l].end(), 
                   d[r].begin(), d[r].end(), 
                   d[k].begin());
    }
};



template <class S,
          S (*op)(S, S),
          S (*e)(),
          class F,
          S (*mapping)(F, S),
          F (*composition)(F, F),
          F (*id)()>
struct lazy_segtree {
  public:
    lazy_segtree() : lazy_segtree(0) {}
    lazy_segtree(int n) : lazy_segtree(std::vector<S>(n, e())) {}
    lazy_segtree(const std::vector<S>& v) : _n(int(v.size())) {
        log = 0;
        while ((1 << log) < _n) log++;
        size = 1 << log;
        d = std::vector<S>(2 * size, e());
        lz = std::vector<F>(size, id());
        for (int i = 0; i < _n; i++) d[size + i] = v[i];
        for (int i = size - 1; i >= 1; i--) {
            update(i);
        }
    }

    void set(int p, S x) {
        assert(0 <= p && p < _n);
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        d[p] = x;
        for (int i = 1; i <= log; i++) update(p >> i);
    }

    S get(int p) {
        assert(0 <= p && p < _n);
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        return d[p];
    }

    S prod(int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return e();

        l += size;
        r += size;

        for (int i = log; i >= 1; i--) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push(r >> i);
        }

        S sml = e(), smr = e();
        while (l < r) {
            if (l & 1) sml = op(sml, d[l++]);
            if (r & 1) smr = op(d[--r], smr);
            l >>= 1;
            r >>= 1;
        }

        return op(sml, smr);
    }

    S all_prod() { return d[1]; }

    void apply(int p, F f) {
        assert(0 <= p && p < _n);
        p += size;
        for (int i = log; i >= 1; i--) push(p >> i);
        d[p] = mapping(f, d[p]);
        for (int i = 1; i <= log; i++) update(p >> i);
    }

    // [l, r)
    void apply(int l, int r, F f) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return;

        l += size;
        r += size;

        for (int i = log; i >= 1; i--) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }

        {
            int l2 = l, r2 = r;
            while (l < r) {
                if (l & 1) all_apply(l++, f);
                if (r & 1) all_apply(--r, f);
                l >>= 1;
                r >>= 1;
            }
            l = l2;
            r = r2;
        }

        for (int i = 1; i <= log; i++) {
            if (((l >> i) << i) != l) update(l >> i);
            if (((r >> i) << i) != r) update((r - 1) >> i);
        }
    }

    template <bool (*g)(S)> int max_right(int l) {
        return max_right(l, [](S x) { return g(x); });
    }
    template <class G> int max_right(int l, G g) {
        assert(0 <= l && l <= _n);
        assert(g(e()));
        if (l == _n) return _n;
        l += size;
        for (int i = log; i >= 1; i--) push(l >> i);
        S sm = e();
        do {
            while (l % 2 == 0) l >>= 1;
            if (!g(op(sm, d[l]))) {
                while (l < size) {
                    push(l);
                    l = (2 * l);
                    if (g(op(sm, d[l]))) {
                        sm = op(sm, d[l]);
                        l++;
                    }
                }
                return l - size;
            }
            sm = op(sm, d[l]);
            l++;
        } while ((l & -l) != l);
        return _n;
    }

    template <bool (*g)(S)> int min_left(int r) {
        return min_left(r, [](S x) { return g(x); });
    }
    template <class G> int min_left(int r, G g) {
        assert(0 <= r && r <= _n);
        assert(g(e()));
        if (r == 0) return 0;
        r += size;
        for (int i = log; i >= 1; i--) push((r - 1) >> i);
        S sm = e();
        do {
            r--;
            while (r > 1 && (r % 2)) r >>= 1;
            if (!g(op(d[r], sm))) {
                while (r < size) {
                    push(r);
                    r = (2 * r + 1);
                    if (g(op(d[r], sm))) {
                        sm = op(d[r], sm);
                        r--;
                    }
                }
                return r + 1 - size;
            }
            sm = op(d[r], sm);
        } while ((r & -r) != r);
        return 0;
    }

  private:
    int _n, size, log;
    std::vector<S> d;
    std::vector<F> lz;

    void update(int k) { d[k] = op(d[2 * k], d[2 * k + 1]); }
    void all_apply(int k, F f) {
        d[k] = mapping(f, d[k]);
        if (k < size) lz[k] = composition(f, lz[k]);
    }
    void push(int k) {
        all_apply(2 * k, lz[k]);
        all_apply(2 * k + 1, lz[k]);
        lz[k] = id();
    }
};

struct node {
    ll sum, count;
};

struct lazy_node {
    bool assign;
    ll add_x, assign_x;
};

node tree_op(node a, node b) {
    return {a.sum + b.sum, a.count + b.count};
}
node tree_e() { return {0, 0}; }
// merge lazy_node in node
node tree_mapping(lazy_node f, node s) {
    ll base = f.assign ? f.assign_x * s.count : s.sum;
    return {base + f.add_x * s.count, s.count};
}
// merge lazy_node in lazy_node f(s(x))
lazy_node tree_composition(lazy_node f, lazy_node s) {
    if(f.assign)
        return f;

    return {s.assign, f.add_x + s.add_x, s.assign_x};
}
lazy_node tree_id() {return {false, 0, 0};}

using lazy_segtree_op = lazy_segtree<node, tree_op, tree_e, lazy_node, tree_mapping, tree_composition, tree_id>;
