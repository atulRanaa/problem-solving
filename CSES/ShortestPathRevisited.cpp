#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll N = 1e5 + 5;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define pii pair<int,int>
#define pis pair<ll,string>
#define F first
#define S second
#define LCM(a,b) ((a*b)/__gcd(a,b))
#define inf 1e15
#define test ll cse;cin>>cse;for(ll _i=1;_i<=cse;_i++)
#define PI 3.14159265
#define fast ios_base::sync_with_stdio(false);cin.tie(NULL);cout.tie(NULL);
const double EPS = 1E-9;
typedef vector< vector<double> > matrix;
typedef vector<int> vi;

using namespace std;

struct edge {
    int v;
    ll w;
};

vector< edge > graph[N];
ll dist[N];
vector< edge > parent(N);
bool visited[N];

vector< int > paths[N];
vector< ll > weight[N];

vector< ll > ans(N);


int n, m, K;

bool operator <( edge a, edge b ) {
    return a.w < b.w;
}

void init() {
    for(int i = 0; i < N; ++i) dist[i] = inf, visited[i] = false, paths[i].clear(), ans[i] = inf;
}


void printPath(int node, int dest) {
    // Base Case : If j is source
    if (parent[ node ].v == - 1) return;
    printPath(parent[ node ].v, dest);

    paths[ dest ].push_back( node );
    weight[dest ].push_back( parent[node].w);
    // cout << node << " ";
}

void calculateWeight(int node) {
    paths[node].clear();
    weight[node].clear();

    // order mixed
    printPath(node, node);

    ll W = 0;
    sort(weight[node].begin(), weight[node].end(), greater< ll > ());
    for(int j = K; j < weight[node].size(); j++) W += weight[node][j];


    cout << "W:" << W << "\n";
    ans[ node ] = min( ans[node], W);
}

void dijkstra(int S) {
    int i;

    priority_queue< edge > q;
    q.push( ( edge ) { S, 0 } );
    dist[ S ] = 0;

    while ( !q.empty() ) {
        edge p = q.top();
        q.pop();

        // cout << p.v << "\n";
        visited[ p.v ] = true;

        for ( i = 0; i < graph[ p.v ].size(); ++i ) {
            edge k = graph[ p.v ][ i ];
            if (dist[ p.v ] + k.w < dist[ k.v ] ) {

                dist[ k.v ] = dist[ p.v ] + k.w;
                parent[ k.v ] = (edge) {p.v, k.w};

                // calculateWeight(k.v);
                q.push( k );
            }
        }
    }
}


int main() {
    fast;

    cin >> n >> m >> K;

    int u, v;
    ll w;
    for (int i = 0; i < m; ++i ) {
        cin >> u >> v >> w;
        graph[ u ].push_back( ( edge ) { v, w } );
        graph[ v ].push_back( ( edge ) { u, w } );
    }

    init();

    parent[1] = (edge) {-1, 0};
    dijkstra(1);

    // for(int i = 1; i <= n; i++) cout << "Path of " << i << ":", printPath(i, i), cout << "\n";

    for(int i = 1; i <= n; i++) {

        paths[i].clear(), weight[i].clear();
        printPath(i, i);
    }

    for(int i = 1; i <= n; i++) {

        ll W = 0;
        // cout << i << "->";
        // for(int j = 0; j < paths[i].size(); j++) cout << paths[i][j] << " " << weight[i][j] << ": ";
        // order mixed
        sort(weight[i].begin(), weight[i].end(), greater< ll > ());
        for(int j = K; j < weight[i].size(); j++) W += weight[i][j];

        // cout << W << "\n";
        cout << W << " ";

    }
    cout << "\n";

    // ans[1] = 0;
    // for(int i = 1; i <= n; i++) cout << ans[i] << " "; cout << "\n";
    return 0;
}


/*

9 14 1
1 2 4
1 8 8
2 3 8
2 8 11
3 4 7
3 6 4
3 9 2
4 5 9
4 6 14
5 6 10
6 7 2
7 8 1
7 9 6
8 9 7

*/