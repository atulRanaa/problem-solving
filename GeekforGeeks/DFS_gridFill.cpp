#include <bits/stdc++.h>
using namespace std;

#define F first
#define S second
vector<string> grid;
bool visited[10001][10001];
int M,N;

bool safe(int i,int j){
	return (i>=0 && j>=0 && i<M && j<N);
}
pair<bool,int> dfs(int i,int j){

    if(grid[i][j]=='*') return {true,0};
    
    if(!safe(i-1,j)) return {false,0};
    if(!safe(i+1,j)) return {false,0};
    if(!safe(i,j-1)) return {false,0};
    if(!safe(i,j+1)) return {false,0};

    visited[i][j] = true;
    pair<bool,int> f1,f2,f3,f4;
    f1=f2=f3=f4={true,0};
    if(safe(i-1,j) && !visited[i-1][j]) f1=dfs(i-1,j);
    if(safe(i+1,j) && !visited[i+1][j]) f2=dfs(i+1,j);
    if(safe(i,j-1) && !visited[i][j-1]) f3=dfs(i,j-1);
    if(safe(i,j+1) && !visited[i][j+1]) f4=dfs(i,j+1);

    int ans = 1+f1.S+f2.S+f3.S+f4.S;
    if(f1.F && f2.F && f3.F && f4.F) return {true,ans};
    else return {false,0};
}

int volume_trapped (int m,int n) {
    memset(visited, false, sizeof(visited));
    
    int ans = 0;
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            if( !visited[i][j] && grid[i][j] =='.'){
                pair<bool, int> f = dfs(i,j);
                ans = max(ans, f.S);
            }
        }
    }
    return ans;
}

int main() {

    ios::sync_with_stdio(0);
    cin.tie(0);
    
    cin >> M >> N;

    for(int i_grid=0; i_grid<M; i_grid++)
    {
        string s;
        cin >> s;

        grid.push_back(s);
    }

    
    int out_;
    out_ = volume_trapped(M,N);
    cout << out_<<"\n";
    return 0;
}
