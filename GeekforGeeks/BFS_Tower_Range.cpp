#include <bits/stdc++.h>
using namespace std;

int main()
{
    //cout << "Hello World!" << endl;
    int n,m,t; cin>>n>>m>>t;
    
    int x,y;
    vector<int> edges[n+1];
    for(int i=0;i<m;i++){
        cin>>x>>y;
        edges[x].push_back(y);
        edges[y].push_back(x);
    }
    vector<bool> visit(n+1,false);
    
    int tower[100005]={0};
    int tow[t],ran;
    
    for(int i=0;i<t;i++){ 
        cin>>tow[i]>>ran;
        tower[ tow[i] ] = ran;
    }
    
    for(int j=0;j<t;j++){
        queue<int> q;
        q.push(tow[j]);
        
        while(!q.empty()){
            int node = q.front();
            q.pop();
            visit[node] = true;
            if(tower[node]-1 >= 0){
		    for(int i=0;i<edges[node].size();i++){
		           
		           if(tower[node]-1 >= tower[ edges[node][i] ]){
		           	q.push( edges[node][i]);
		           	visit[ edges[node][i] ] = true;		           	
		           	tower[ edges[node][i]] = tower[node]-1;
		           }
		    }
	    }
        }
    }
    int ans =0;
    for(int i=1;i<=n;i++){
        if(!visit[i]) ans++;
    }
    cout<<ans<<"\n";
    for(int i=1;i<=n;i++){
        if(!visit[i]) cout<<i<<" ";
    }
    return 0;
}

