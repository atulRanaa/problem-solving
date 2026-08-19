#include<bits/stdc++.h>
using namespace std;

vector<int> adj[100005];
vector<int> bit[100005];
int bitSum[25] = {0};
bool visited[100005];

int ans = 0;
bool TreeOR(int a){
    bool f = true;
    int b[25]={0};
    for(int i=0;i<20;i++) b[i] = bitSum[i]-bit[a][i];
    for(int i=0;i < 20;i++){
        if((bit[a][i]==0 && b[i]!=0) || (bit[a][i]!=0 && b[i]==0)){
            f = false;
            break;
        }
    }
    return f;
}
void dfs(int src,int par){

    for(int i=0;i < adj[src].size(); i++){
        int v = adj[src][i];
        if(v != par){

            dfs(v, src);
            for(int j=0;j<20;j++){
                bit[src][j] += bit[v][j];
            }

        }
    }
    if( TreeOR(src) ) ans++;
}

int main(){
    
    int n;    cin>>n;
    int ar[n+5];
    for(int i=1;i <= n; i++) cin>>ar[i];
    int x,y;
    for(int i=1;i<=n-1;i++){
        cin>>x>>y;
            adj[x].push_back(y);
            adj[y].push_back(x);
    }
    for(int i=1;i<=n;i++){
        int num = ar[i];
        for(int j=1;j<=20;j++){
            
            
            if(num&1) bit[i].push_back(1);
            else bit[i].push_back(0);

            num = num>>1;
        }
    }
    /*for(int i=1;i <=n;i++){
        for(int j=0;j<20;j++) cout<<bit[i][j];
        cout<<"\n";
    }
    cout<<"\n";*/
    for(int i=1;i <=n;i++){
        for(int j=0;j<20;j++)
            bitSum[j] += bit[i][j];
    }

    dfs(1,0);
    /*for(int i=0;i<20;i++) cout<<bitSum[i];
    cout<<"\n";
    for(int i=1;i <=n;i++){
        for(int j=0;j<20;j++) cout<<bit[i][j];
        cout<<"\n";
    }*/
    cout<<ans;
    return 0;
}
