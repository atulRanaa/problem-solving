#include <cstdio>
#include <cassert>
#include <queue>
#include <utility>
using std::queue;
using std::pair;

const int MAXN = 100005;
const int MAXM = MAXN * 2;

struct Edge {
  int r, c, next;
  Edge() {}
  Edge(int r, int c, int next) : r(r), c(c), next(next) {}
} ln[MAXM];
int x[MAXN], re[MAXN], dep[MAXN];
int dis[MAXN], st[MAXN], ed[MAXN], go[MAXN][17];
int bit[MAXN];
int tot, n, dfsid;

inline int Lowbit(int x) { return x & -x; }

void Add(int l, int r, int c) {
  ln[tot] = Edge(r, c, x[l]);
  x[l] = tot++;
  ln[tot] = Edge(l, c, x[r]);
  x[r] = tot++;
}

void DFS(int p, int depth, int d, int par) {
  // DFS the tree to find the start and end position of each subtree in the DFS sequence.
  st[p] = dfsid++;
  dis[p] = d;
  dep[p] = depth;
  go[p][0] = par;
  for (int i = x[p]; i; i = ln[i].next) {
    if (ln[i].r == par) {
      re[i >> 1] = p;
    } else {
      DFS(ln[i].r, depth + 1, d + ln[i].c, p);
    }
  }
  ed[p] = dfsid;
}

void Init() {
  tot = 2;
  assert(scanf("%d", &n) == 1);
  for (int i = 1; i <= n; ++i) {
    x[i] = re[i] = dep[i] = dis[i] = st[i] = ed[i] = 0;
    bit[i] = go[i][0] = 0;
  }
  for (int i = 1; i < n; ++i) {
    int u, v, c;
    assert(scanf("%d%d%d", &u, &v, &c) == 3);
    Add(u, v, c);
  }
  dfsid = 1;
  DFS(1, 0, 0, 0);
  // Prepare for LCA queries.
  for (int j = 1; j < 17; ++j) {
    for (int i = 1; i <= n; ++i) {
      go[i][j] = go[go[i][j - 1]][j - 1];
    }
  }
}

// BIT
void Insert(int x, int v) {
  for ( ; x <= n; x += Lowbit(x))
    bit[x] += v;
}

// BIT
int GetSum(int x) {
  int ret = 0;
  for ( ; x; x -= Lowbit(x))
    ret += bit[x];
  return ret;
}

int GetLCA(int u, int v) {
  if (dep[u] < dep[v]) std::swap(u, v);
  int diff = dep[u] - dep[v];
  for (int i = 0; diff; ++i, diff >>= 1) {
    if (diff & 1) {
      u = go[u][i];
    }
  }
  if (u == v) return u;
  for (int i = 16; i >= 0; --i) {
    if (go[u][i] != go[v][i]) {
      u = go[u][i];
      v = go[v][i];
    }
  }
  return go[u][0];
}

void Query1(int i, int c) {
  int diff = c - ln[i * 2].c;
  ln[i * 2].c = c;
  ln[i * 2 + 1].c = c;
  Insert(st[re[i]], diff);
  Insert(ed[re[i]], -diff);
}

int Query2(int u, int v) {
  int w = GetLCA(u, v);
  int ret = dis[u] + dis[v] - dis[w] * 2;
  ret += GetSum(st[u]);
  ret += GetSum(st[v]);
  ret -= GetSum(st[w]) * 2;
  return ret;
}

void Work() {
  int q;
  assert(scanf("%d", &q) == 1);
  for (int t = 1; t <= q; ++t) {
    int type, u, v;
    assert(scanf("%d%d%d", &type, &u, &v) == 3);
    if (type == 1) {
      Query1(u, v);
    } else if (type == 2) {
      printf("%d\n", Query2(u, v));
    } else {
      assert(0);
    }
  }
}

int main() {
  int cases = 1;
  while (cases--) {
    Init();
    Work();
  }
  return 0;
}
