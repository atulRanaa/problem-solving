/*
Bangalore Hyderabad
Bangalore Hyderabad 10000
Bangalore Chennai 4000
Chennai Hyderabad 4000
Bangalore Aodai 4000
Aodai Hyderabad 4000
*/

#include <bits/stdc++.h>
using namespace std;

const int N = 500;

struct edge {
    int v;
    int w;
};

unordered_map< string, int > strMap;
unordered_map< int, string > intMap;
int cnt = 0;

vector< edge > graph[N];
bool visited[N];

int path[N], weight[N];
int path_index;
vector< vector<int> > paths;

void travelAllPaths(edge node, int dest, int &path_index) {
    visited[ node.v ] = true;
    path[ path_index ] = node.v; weight[ path_index ] = node.w;
    path_index++;

    if(node.v == dest) {
        int sumOfW = 0;
        for(int i=0;i < path_index; ++i) sumOfW += weight[i];
        vector< int > itr;

        itr.push_back(sumOfW);
        itr.push_back(path_index);
        for(int i=0;i < path_index; ++i) itr.push_back( path[i] );
        paths.push_back( itr );
    }
    else {
        int len = graph[ node.v ].size();
        for(int i=0;i < len; ++i) {
            edge e = graph[ node.v ][i];
            if(!visited[ e.v ])
                travelAllPaths(e, dest, path_index);
        }
    }

    path_index--;
    visited[ node.v ] = false;
}

int main() {
    string source, destination;
    cin >> source >> destination;

    vector< pair<string, pair< string, int> > > edges;
    string x, y; int w;
    while(cin >> x >> y >> w) {
        // cout << x << y << w << "\n";
        edges.push_back( { x, { y, w } } );
        strMap[x] = ++cnt;
        strMap[y] = ++cnt;
    }
    for(auto e: strMap) intMap[e.second] = e.first;
    // for(auto e: intMap) cout << e.first << " " << e.second << "\n";
    for(auto e: edges) {
        int p = strMap[ e.first ];
        int q = strMap[ e.second.first ];
        // cout << p << " " << q << "\n";
        graph[p].push_back( (edge){ q, e.second.second } );
    }

    int s = strMap[ source ], d = strMap[ destination ];
    // cout << s << " " << d << "\n";
    path_index = 0;
    travelAllPaths((edge){s, 0}, d, path_index);


    // sort(paths.begin(), paths.end());
    // for(auto path: paths) { for(auto e: path) cout << e << " "; cout << "\n";}


    map< pair<int, int>, vector<int> > sortBy;

    if(paths.size() == 0) cout << "No Trains";
    else {
        int l = paths.size();
        for(int i=0;i<l; ++i) {
            int w = paths[i][0];
            int conn = paths[i][1];
            sortBy[ {w, conn} ].push_back(i);
        }
        for(auto e: sortBy) {
            // cout << e.first.first << " " << e.first.second << "\n";
            // for(int index: e.second) cout << index << " "; cout << "\n";

            vector< vector< string > > names(e.second.size());
            int j = 0;
            for(int index: e.second) {
                int len = paths[index].size();
                for(int i=2;i<len;++i) {
                    int strID = paths[index][i];
                    names[j].push_back( intMap[ strID ]);
                }
                j++;
            }
            sort(names.begin(), names.end());

            // print paths
            for(auto p: names) { 
                for(string n: p) cout << n << " "; cout << e.first.first <<"\n";
            }
        }
    }
    
}