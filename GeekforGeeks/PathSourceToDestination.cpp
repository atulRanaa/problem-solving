#include<bits/stdc++.h>
using namespace std; 

		typedef long long ll; 
        const ll N = 1e6+5;
        const ll MOD = 1000000007;
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

struct edge {
    int v;
    int w;
};
bool operator <( edge a, edge b ) {
    return a.w < b.w;
}

vector< edge > graph[N];
bool visited[N];

int n, m;
int source, destination;
int path[N], weight[N];
int path_index;
vector< vector<int> > paths;

void printAllPaths(edge node, int dest, int &path_index) { 
	visited[ node.v ] = true; 
	path[path_index] = node.v; weight[path_index] = node.w;
	path_index++;

	if (node.v == dest) { 
		vector< int > itr;
		int totalWeight = 0;
		for (int i = 0; i < path_index; i++) totalWeight += weight[i];

		itr.push_back(totalWeight);
		for (int i = 0; i < path_index; i++) itr.push_back( path[i] );
		paths.push_back( itr );		
	}
	else { 
		for(int i = 0; i < graph[ node.v ].size(); ++i) {
			edge connNode = graph[ node.v ][i];
			if (!visited[ connNode.v ])
				printAllPaths( connNode, dest, path_index); 
		}
	}

	path_index--; 
	visited[ node.v ] = false; 
} 

void init() {
	memset(visited, false, sizeof visited);
}

/*
4 6
3 4
1 2 1 
1 3 2
1 4 1
3 1 1
3 2 3
2 4 2
*/
int main() {
	init();
	cin >> n >> m;
	cin >> source >> destination;

	int x, y, w;
	for(int i=0;i<m;i++){
		cin >> x >> y >> w;
		graph[x].push_back( (edge) {y, w} );
	}
	path_index = 0;
	printAllPaths((edge){source, 0}, destination, path_index);

	for(auto path: paths) {for(int node: path) cout << node << "=>"; cout << "\n";}
	return 0; 
} 
