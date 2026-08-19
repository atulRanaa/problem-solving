#include<bits/stdc++.h>
using namespace std;
const int N=1002;
#define pb      push_back
#define mp      make_pair
vector<int> tree[N];
vector<pair<int,int> > ans;
bool used[N];

void dfs(int u,int pa,int di){
	if(used[u])  ans.pb(mp(di,u));
        for(int i=0;i<tree[u].size();i++){
                int v=tree[u][i];
                if(v==pa) continue;
                dfs(v,u,di+1);
        }
}
int main()
{       int n;  cin>>n;
        
        int x,y;
        for(int i=1;i<=n-1;i++){       
                cin>>x>>y;
                tree[x].pb(y);
                tree[y].pb(x);
        }
        int q; cin>>q;
        while(q--){
        	cin>>x;
                used[x]=true;
        }
        
        dfs(1,0,0);
        sort(ans.begin(),ans.end());
        cout<<ans[0].second<<endl;
}
