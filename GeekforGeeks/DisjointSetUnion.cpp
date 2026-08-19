#include<bits/stdc++.h>
using namespace std;

#define ll long long int
#define all(x) x.begin(),x.end()

const int MAX = 1e5 + 5;
int id[MAX];
int size[MAX];

void initialize(int n){
    for(int i = 0;i <=n;++i){   
         id[i] = i;
         size[i]=1;
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
    int p = findset(x);
    int q = findset(y);
    
    if(p==q) return;
        
    if(size[p]>=size[q]){
        id[q]=p; 
        size[p]+=size[q];
        size[q]*=-1;
    }
    else{
        id[p]=q; 
        size[q]+=size[p];
        size[p]*=-1;
    }
}

int main(){
    
    int n,e; cin>>n>>e;
    int x,y;
    initialize(MAX);
    for(int i=1;i<=e;i++){
        cin>>x>>y;
        combine(x,y);
        
        vector<int> ans;
        for(int i=1;i<=n;i++) if(size[i]>0)	ans.push_back(size[i]);
        sort(all(ans));
        
        for(int i=0;i<ans.size();i++) cout<<ans[i]<<" ";
        cout<<"\n";
    }
    return 0;
}
