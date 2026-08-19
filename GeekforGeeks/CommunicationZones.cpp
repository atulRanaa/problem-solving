
// Tech Challenge Capgemini

#include <bits/stdc++.h>
#define all(x) x.begin(),x.end()
#define F first
#define S second

using namespace std;
typedef long long ll;
typedef vector<int> vi;
typedef pair<int,int> pii;


const int MAX = 1e6 + 5;
int id[MAX];
int sz[MAX];

// multiset<int> sett;
// vector< vector< pii > > adj(MAX);
// vector< pii > nodes;
// bool visited[MAX];


void initialize(int n){
    //sett.clear();
    // adj.clear();
    // nodes.clear(); 
    // memset(visited, false, sizeof visited);
    
    for(int i = 1;i <=n;++i){   
         id[i]=i;
         sz[i]=1; //sett.insert(1);
     }
}
int findset(int x){
    while(id[x] != x){
        id[x] = id[id[x]];
        x = id[x];
    }
    return x;
}
 
void combine(int x, int y){
    int p = findset(x), q = findset(y);
    if(p==q) return;
    if(sz[q] >sz[p]) swap(p,q);
    
        //sett.erase( sett.find(sz[p])); sett.erase( sett.find(sz[q]));
        id[q]=p; sz[p]+=sz[q];

        sz[q]=-1;
        //sett.insert(sz[p]);
}
// void dfs(int v,int W){
//     visited[v] = 1;
//     for(auto e: adj[v]){
//         if(!visited[e.F]){
//             nodes.push_back({e.S+W, e.F}); dfs(e.F, e.S+W);
//         }
//     }
// }
int G[1005][1005];
int rw, cl;
void input(){
    memset(G, 0, sizeof G);
    string mat;

    cin>>mat;

    istringstream st1(mat);
    string row;
    int i=1;
    while( getline(st1, row, '#')){

        istringstream st2(row);
        string col;
        int val, j =1;
        while( getline(st2, col, '@')){
            stringstream num(col);
            num >> val;
            G[i][j] = val;
            j++;
        }
        cl = j;
        i++;
    }
    rw = i;
}
int _x[]={-1,-1,-1, 0,0, 1,1,1};
int _y[]={-1, 0, 1,-1,1,-1,0,1};

bool valid(int x,int y){
    return (x>=1 && x<rw)&&(y>=1 && y<cl);
}
int main(){
    input();
    cout<< rw <<" "<<cl<<endl;
    initialize((rw-1)*(cl-1));
    cout<<(rw-1)*(cl-1) <<endl;
    
    for(int i=1;i<rw;i++) {for(int j=1;j<cl;j++) cout<<G[i][j]<<" "; cout<<"\n";}
    
    int ind=1;
    vector< pair<int, pair<int,int> > > v;
    for(int i=1;i<rw;i++)for(int j=1;j<cl;j++){

        if(G[i][j]==-1){
            
            ind = i*(rw-1)+j;
            for(int p=0;p<8;p++){
                    int X=i+_x[p], Y=j+_y[p];
                    if(valid(X,Y) && G[X][Y]==-1){
                        int t = X*(rw-1)+Y;
                        combine(ind,t);
                    }
            }
        }
        else
            v.push_back({G[i][j],{i,j}});
    }
    sort(v.begin(), v.end());
    
    cout<< v.size()<<"\n";
    // if(sett.size() == 1) cout<<0;
    // else
    //     {   
    //         long long cost=0;
    //         for(auto e: v){
    //             int val = e.first;
    //             int i=e.second.first, j = e.second.second;
    //             //cout<<val << " "<<i<<" "<<j<<"\n";
    //             cost += val;
    //             G[i][j]=-1;
    //             int ind = i*(rw-1)+j;

    //             for(int p=0;p<8;p++){
    //                 int X=i+_x[p], Y=j+_y[p];
    //                 if(valid(X,Y) && G[X][Y]==-1){
    //                     int t = X*(rw-1)+Y;
    //                     cout<<X<<" "<<Y<<" : ";
    //                     combine(ind,t);
    //                 }
    //             }
    //             cout<<"\n";
    //             cout<< findset(ind) << " " << sz[findset(ind)]<<"\n";
                
    //         }
    //         cout<< cost;
    //     }
    return 0;
}