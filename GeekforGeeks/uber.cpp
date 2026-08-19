#include <bits/stdc++.h>
using namespace std;

/*string solve(int A, vector<string> &B) {
    unordered_map< string, long> m;
    long num = -10000000000;
    string name = "";
    for(int i=0;i<A;i += 2){
        

        stringstream g(B[i+1]);
        long val;
        g >> val;
        cout<< val<<"\n";
        m[ B[i]] += val;
        long t = m[ B[i]];
        
        
        
        if(t > num){
            num = m[ B[i]];
            name = B[i];
        }
        else if(B[i] == name){
            num = t;
        }
    }
    return name;
}*/

vector<int> adj[1000006];
int visited[1000006]={0};
int rot[1000006]={0};



void dfs(int node){
    
    visited[node] = 1;
    bool f = false;
    for(auto next: adj[node]){
        if(!visited[next]){
            dfs(next);
        }
        else{
            rot[next] = next;
        }
    }
    for(auto next: adj[node])
    if(f) rot[node] = 1;
}
void solve(int A, int B, vector<vector<int> > &C) {
    for(auto ele: C){
        adj[ ele[0]].push_back(ele[1]);
    }
    for(int i=1; i<=A;i++)
        if(!visited[i]) dfs(i);
}

int main(){
    vector< vecotr<int> > s = {"a","5","a","15","a","20","a","-1000","cd","5"};
    cout<< solve(5,5,s);
    return  0;
}
