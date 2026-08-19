#include <bits/stdc++.h>
#define ll long long
#define T ll t; cin>>t; while(t--)
using namespace std;
#define speed ios_base::sync_with_stdio(0)
#define endl "\n"

vector<pair<int,int>>adj[1000001];
ll c=1000000000;
bool visited[1000001];
ll dist[1000001];

void intilization(int n)
{
    for(ll i=0;i<=n;i++){
        visited[i]=0;
        dist[i]=c;
    }
}

void dijkstras(ll center){
	ll st=center;
	dist[st]=0;
	multiset< pair<ll,int> >s;
	s.insert({0,st});
	while(!s.empty())
	{
		ll from,to,cost;
		pair<ll,int> p = *s.begin();
		s.erase(s.begin());
		from = p.second;
		if(visited[from]) continue;
		visited[from]=1;
		for(ll i=0;i<adj[from].size();i++)
		{
			cost=adj[from][i].first;
			to=adj[from][i].second;
			if(dist[from]+cost<dist[to])
			{
				dist[to]=dist[from]+cost;
				s.insert({dist[to],to});
			}

		}
	}
}

int main(){
	speed;
	int t; cin>>t;
	while(t--){
		ll nodes,edges,i;
		cin>>nodes>>edges;
		intilization(nodes);
		for(i=0;i<edges;i++)
		{
			ll from,to,cost;
			cin>>from>>to>>cost;
			adj[from].push_back({cost,to});
		}
			ll center,total; 
			cin>>center>>total;
			dijkstras(center);
			sort(dist,dist+nodes+1);
			int count=0;
			int j=0;
			while(total-dist[j] >= 0){
			    total -= dist[j];
			    count++;
			    j++;
			}
		cout<<count<<endl;
	}
	return 0;
}
