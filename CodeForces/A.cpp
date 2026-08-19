#include <bits/stdc++.h>
using namespace std;

const int MAX = 1e5 + 5;
int id[MAX];
long long sz[MAX];


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
    if(sz[q] > sz[p]) swap(p,q);
    
        id[q]=p; sz[p]+=sz[q];
        sz[q]=-1;
}
int main(){


    int n; cin>>n;
    int m; cin>>m;


    for(int i = 1;i <=n;++i){   
         id[i]=i; sz[i]=1; 
     }

    int a,b;
    for(int i=0;i<m;i++){
        cin>> a>> b; combine(a,b);
    }
    long long int ans=0;
    for(int i=1;i<=n;i++) if(id[i]==i){
        ans += ((sz[i]*(sz[i]-1))/2);
    }

    cout<<ans<<"\n";
    return 0;
}