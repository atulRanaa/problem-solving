#include "bits/stdc++.h"
using namespace std;


vector<int> adj[100005];
int del, leaf=0;

void dfs(int cur,int par){
	if(cur != del){
		for(auto x: adj[cur]){
			if(x != par) dfs(x,cur);
		}
		if(adj[cur].size() == 1) leaf++;
	}
}

int main(){
	int n; scanf("%d",&n);
	int ar[n]; for(int i=0;i<n;i++) scanf("%d",&ar[i]);
	scanf("%d",&del);

	int root;
	for(int i=0;i<n;i++){

		if(ar[i] == -1) root = i;
		else
			adj[i].push_back(ar[i]), adj[ ar[i] ].push_back(i);
	}
	dfs(root,-1);
	printf("%d\n",leaf);
}