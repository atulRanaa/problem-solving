#include<bits/stdc++.h>
using namespace std;
 
#define X first
#define Y second
 
typedef long long ll;
typedef vector<int> vi;
typedef vector<vi> vvi;
typedef pair<int,int> pii;
 
template<typename T> struct HeavyLightDecomposition {
  
  int n; 
  vi heavy, head, par, pos, level;
  vector<T> cost;
  vector<vector<pair<int,T>>> adj;
  
  int dfs(int u, int p, int d) {
    int size = 1, max_child = 0, max_child_id = -1;
    par[u] = p, level[u] = d;
    
    for (auto& child : adj[u]) if (child.X != p) {
        cost[child.X] = child.Y;
        int child_size = dfs(child.X, u, d + 1);
        if (child_size > max_child) max_child = child_size, max_child_id = child.X;
        size += child_size;
    }
    if (max_child * 2 >= size) heavy[u] = max_child_id;
    return size;
  }
  HeavyLightDecomposition(int n) : n(n), heavy(n), head(n), par(n), pos(n), level(n), cost(n), adj(n) { }
  
  void add_edge(int u, int v, T cost) {
      adj[u].emplace_back(v, cost);
      adj[v].emplace_back(u, cost);
  }
  vector<T> decompose_tree(int root = 0) {  // Perform HLD.
    vector<T> val(n);
    heavy.assign(n, -1);

    dfs(root, -1, 0);
    int curPos = 0;
    for (int i=0, cur=0; i<n; cur=++i)
      if (par[i] == -1 || heavy[par[i]] != i) while (cur != -1)
        val[curPos] = cost[cur], pos[cur] = curPos++, head[cur] = i, cur = heavy[cur];
    return val;
  }
  template<typename F> void ranged_query(int u, int v, F query) {
    while (head[u] != head[v]) {
      if (level[head[u]] > level[head[v]]) swap(u, v);
      query(pos[head[v]], pos[v]);  v = par[head[v]];
    }
    if (u != v) query(min(pos[u],pos[v])+1, max(pos[u],pos[v]));
  }
  template<typename F> void point_query(int u, int v, F query) {
    query(level[v] > level[u] ? pos[v] : pos[u]);
  }

};
 
// Ranged query, point update Fenwick tree
struct FenwickTree2 {
	int N;
    vi A;
	FenwickTree2(int n): N(n+1), A(N) {}
	int rq(int b) { int sum=0; for (;b;b-=b&-b) sum+=A[b]; return sum; }
	int rq(int a,int b) { return rq(b)-rq(a); }
	void adjust(int i,int v) { for (i++;i<N;i+=i&-i) A[i]+=v; }
};
 
void solve_CC_TAQTREE() {
  int N; cin >> N;
  HeavyLightDecomposition<int> hld(N);
  vector<pii> edges;
  for (int i=0; i<N-1; i++) {
    int a, b, c;

    cin >> a >> b >> c; a--, b--;
    hld.add_edge(a,b,c);
    edges.emplace_back(a,b);
  }
  auto arr = hld.decompose_tree();
  
  FenwickTree2 ft(arr.size());
  for (int i=0; i<(int)arr.size(); i++) ft.adjust(i, arr[i]);
  
  int Q; cin >> Q;
  while (Q--) {
    int type; cin >> type;
    if (type == 1) {
      int i, c; cin >> i >> c; i--;
      hld.point_query(edges[i].X, edges[i].Y, [&](int j) {
        int cur = ft.rq(j,j+1);
        ft.adjust(j,c-cur);
      });
    } else {
      int u, v; cin >> u >> v; u--, v--;
      int ans = 0;
      hld.ranged_query(u, v, [&](int i, int j) { ans += ft.rq(i, j+1); });
      cout << ans << '\n';
    }
  }
}
 
int main() {
  ios_base::sync_with_stdio(0); cin.tie(0);
  solve_CC_TAQTREE();
}
     
