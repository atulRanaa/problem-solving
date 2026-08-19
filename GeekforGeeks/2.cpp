#include <bits/stdc++.h>
using namespace std;

int B[1005][1005];
#define pii pair<int,int>
#define F first
#define S second
int p[] = {-2,-1,1,2,2,1,-1,-2};
int q[] = {-1,-2,-2,-1,1,2,2,1};
int N,M;

void bfs(int x,int y,int v){
    queue< pii > Q;
    Q.push({x,y});
    B[x][y] = v;
    
    
    while(!Q.empty()){
        pii Z = Q.front();
        Q.pop();
        
        for(int i=0;i<8;i++){
            int X = (Z.F +p[i]+N)%N;
            int Y = (Z.S +q[i]+M)%M;
            if(B[ X ][ Y ]==0){
                B[X][Y] = B[Z.F][Z.S]+1;
                Q.push({X,Y});
            }
        }
    }
}
int main(){
    int t; cin>>t;
    while(t-- > 0){
        cin>>N>>M;
        int x,y; cin>>x>>y;
        
        memset(B, 0, sizeof(B));
        bfs(x,y,1);
        int ans = 0; bool f = true;
        for(int i=0;i<N;i++){
            for(int j=0;j<M;j++){
                if(B[i][j] > ans) ans = B[i][j];
                if(B[i][j] == 0 ){
                    f =false;
                    break;
                }
            }
            if(!f) break;
        }
        if(f) cout<< ans-1<<"\n";
        else cout<< -1 <<"\n";
    }
    return 0;
}