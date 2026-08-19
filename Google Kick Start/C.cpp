#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll N = 3e5 + 5;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define pii pair<ll,ll>
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
typedef vector< vector<ll> > array_2D;
typedef pair<ll, pair<ll, ll> > ppi;
typedef vector<ll> vi;

const int R = 50;
const int C = 50;



char ar[R][C];

int adj[R][C];

vector< int > ans;
int visited[100];
int nodes[26] = {0};

string bfs() {
    int V = 26;
    vector< int > in_degree(V, 0);

    for (int u = 0; u < V; u++) {
        for (int i = 0; i < V; i++) if(adj[u][i] == 1)
            in_degree[i]++;
    }


    multiset<int> s;
    for (int i = 0; i < V; i++) if (in_degree[i] == 0) s.insert(i);

    int cnt = 0;

    vector<int> top_order;

    while (!s.empty()) {

        int u = *s.begin();
        s.erase(s.begin());
        top_order.push_back(u);

        for (int i = 0; i < V; i++) if(adj[u][i] == 1) {
            if (--in_degree[i] == 0)
                s.insert(i);
        }

        cnt++;
    }

    if(cnt != V)
        return "-1";

    string ans;
    int sz = top_order.size();
    for(int i = 0; i < sz; i++) if(nodes[ top_order[i] ] == 1) {
        ans.push_back( (char)(top_order[i] + 'A') );
    }

    return ans;
}

int main() {
    fast;


    test {
        int n, m; cin >> n >> m;
        ans.clear();
        memset(visited, 0, sizeof visited);
        memset(adj, 0, sizeof adj);
        memset(nodes, 0, sizeof nodes);

        for(int i = 0; i < 26; i++) for(int j = 0; j < 26; j++) adj[i][j] = 0;


        for(int i = 0; i < n; i++) {
            for(int j = 0; j < m; j++) cin >> ar[i][j];
        }

        for(int i = 1; i < n; i++) {
            for(int j = 0; j < m; j++) {

                int prev = ar[i-1][j] - 'A';
                int curr = ar[i  ][j] - 'A';

                nodes[prev] = 1;
                nodes[curr] = 1;
                if(prev != curr) adj[ curr ][ prev ] = 1;
            }
        }

        // for(int i = 0; i < 26; i++) {
        //     bool F = false;
        //     for(int j = 0; j < 26; j++) if(adj[i][j]) F = true,cout << (char)(j + 'A') << " ";
        //     if(F) cout << ":" << (char)(i + 'A') << "\n";
        // }

        string ans = bfs();
        cout << "Case #" << _i << ": " << ans << "\n";
    }
    return 0;
}