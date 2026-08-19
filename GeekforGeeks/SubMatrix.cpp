#include <bits/stdc++.h>
using namespace std;


int p[101][5];
bool cross(int i,int j){
        if(p[i][0] >= p[j][2] || p[i][2] <= p[j][0]) return false;
        if(p[i][1] >= p[j][3] || p[i][3] <= p[j][1]) return false;
        return true;
}
int main(){
    int n; scanf("%d",&n);
    
    for(int i=0;i<n;i++)
        for(int j=0;j<5;j++) cin>>p[i][j];
    
    
    long long ans = 0;
    int x,y,X,Y;

    unordered_set< pair<int,int> > S[n];
    for(int i=0;i<n;i++)
        for(int j=i+1;j<n;j++){
            if(cross(i,j)){
                x = max(p[i][0],p[j][0]),y = max(p[i][1],p[j][1]);
                X = min(p[i][2],p[j][2]),Y = min(p[i][3],p[j][3]);
                
                
                for(int _i=x; _i<X; _i++) for(int _j=y; _j<Y;_j++){
                        S[i].add( {_i,_j});
                        S[j].add( {_i,_j});
                }
            }
        }
    for(int i=0;i<n;i++)
        ans += S[i].size()*p[i][4];
    
    printf("%lld", ans);
    return 0;
}