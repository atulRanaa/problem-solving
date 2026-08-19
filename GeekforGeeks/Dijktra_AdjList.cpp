#include<bits/stdc++.h>

using namespace std;

#define ll long long
#define SIZE 100000 + 1

vector < pair < ll , int > > v[SIZE];  
ll dist [SIZE];
ll c=1000000000;
bool vis [SIZE];
void intilization(int n){
    for(ll i=0;i<=n;i++){
        dist[i]=c;
        v[i].clear();
    }
}
void dijkstra(int S){
                                                
    memset(vis, false , sizeof vis);            
    dist[S] = 0;
    multiset < pair< ll , int > > s;          

    s.insert({0 , S});                          

    while(!s.empty()){

        pair < ll , int> p = *s.begin();        
        s.erase(s.begin());

        int x = p.second;
        ll wei = p.first;
        if( vis[x] ) continue;                  
        vis[x] = true;

        for(int i = 0; i < v[x].size(); i++){
            int e = v[x][i].second;
            ll  w = v[x][i].first;
            if(dist[x] + w < dist[e]){            
                dist[e] = dist[x] + w;
                s.insert({dist[e],  e});           
            }
        }
    }
}
int main(){
    freopen("input.txt","r",stdin);
    freopen("output.txt","w",stdout);
    int t; cin>>t;
    
    while(t--){
        
        int nodes,edges,S,D;
        cin>>nodes>>edges>>S>>D;
        ll from,to,cost;
        intilization(nodes);
        for(int i=0; i<edges ;i++){
            
            cin>>from>>to>>cost;
            v[from].push_back({cost,to});
            v[to].push_back({cost,from});
        }
        dijkstra(S);
        
        if(dist[D]!=c) cout<<dist[D]<<endl;
        else cout<<"NONE\n";
    }
    return 0;
}
