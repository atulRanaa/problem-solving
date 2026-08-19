#include <bits/stdc++.h>
using namespace std;

#define F first
#define S second
vector< vector<int> > grid;
int M,N;

bool isP(int n){
    if(n <= 1) return false;
    if(n <= 3) return true;
    if(n%2==0 || n%3==0) return false;

    for (int i=5; i*i<=n; i=i+6)
        if (n%i == 0 || n%(i+2) == 0)
           return false;
}
int solve1(vector< vector<bool> > &g,int m,int n){
    if(m<=0 || n<=0) return 0;
    
    vector< vector<bool> > g1;
    g1.assign(m-1, vector<bool>());

    int count=0;
    for(int i=0;i<m-1;i++){
        for(int j=0;j<n-1;j++){
            if(g[i][j]&&g[i][j+1]&&g[i+1][j]&&g[i+1][j+1]){
                count++;
                g1[i].push_back(1);
            }
            else g1[i].push_back(0);
        }
    }
    return count+solve1(g1,m-1,n-1);
}
int solve0(int m,int n){
    int count =0;
    vector< vector<bool> > g;
    g.assign(m, vector<bool>());

    for(int i=0;i<m;i++) for(int j=0;j<n;j++){
        if( isP(grid[i][j])){
            count++;
            g[i].push_back(1);
        }
        else g[i].push_back(0);
    }
    //for(auto e: g){for(auto ele:e) cout<<ele<<" "; cout<<"\n";}
    return count + solve1(g,m,n);
}
int main() {

    ios::sync_with_stdio(0);
    cin.tie(0);

    cin >> M >> N;
    int temp;
    grid.assign(M, vector<int>());
    
    
    for(int i=0;i<M;i++)for(int j=0;j<N;j++){
        cin>>temp;
        grid[i].push_back(temp);
    }
    for(auto e: grid) {for(auto ele:e) cout<<ele<<" "; cout<<"\n";}
    int ans = solve0(M,N);

    cout<<ans<<"\n";
    return 0;
}
