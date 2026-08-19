#include<bits/stdc++.h>
using namespace std;


#define N 1000
#define M 1000
int n,m;
int ar[N][M];
int visited[N][M];

void init(){
    for(int i=0;i<N;i++) for(int j=0;j<M;j++) visited[i][j]=0;
}
int dfs(int i,int j){
    int count = 1;    
    visited[i][j] = 1;
    if(i-1 >= 0 && ar[i-1][j]==1 && visited[i-1][j] == 0) count+=dfs(i-1,j);
    if(j-1 >= 0 && ar[i][j-1]==1 && visited[i][j-1] == 0) count+=dfs(i,j-1);
    if(i+1 < n  && ar[i+1][j]==1 && visited[i+1][j] == 0) count+=dfs(i+1,j);
    if(j+1 < m  && ar[i][j+1]==1 && visited[i][j+1] == 0) count+=dfs(i,j+1);
    
    if(i-1>=0 && j-1>=0  && ar[i-1][j-1]==1 && visited[i-1][j-1]==0) count+=dfs(i-1,j-1);
    if(i-1>=0 && j+1 <m  && ar[i-1][j+1]==1 && visited[i-1][j+1]==0) count+=dfs(i-1,j+1);
    if(i+1 <n && j-1>=0  && ar[i+1][j-1]==1 && visited[i+1][j-1]==0) count+=dfs(i+1,j-1);
    if(i+1 <n && j+1 <m  && ar[i+1][j+1]==1 && visited[i+1][j+1]==0) count+=dfs(i+1,j+1);
                                    
    return count;
}



int main(){
    init();
    cin>>n>>m;
    for(int i=0;i<n;i++) for(int j=0;j<m;j++) cin>>ar[i][j];
    //for(int i=0;i<n;i++) cout<<ar[i]<<"\n";

    int ans =0;
    for(int i=0;i<n;i++){ for(int j=0;j<m;j++){
            if(ar[i][j] == 1 && visited[i][j] == 0) ans = max(ans, dfs(i,j));
        }
    }
    cout<<ans;
    return 0;

}