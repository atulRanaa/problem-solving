#include<bits/stdc++.h>

using namespace std;

#define ll long long int
#define all(x) x.begin(),x.end()

const int MAX = 1e3 + 5;
int id[MAX];
int size[MAX]; // global variable initialize as 0 for local use memset()

void initialize(int n){
    for(int i = 0;i <n;++i){   
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
        id[q]=p; //p is parent of q
        size[p]+=size[q];
    }
    else{
        id[p]=q; //q is parent of p
        size[q]+=size[p];
    }
}



int main(){
    int n,m,x,y;
    cin >> n>>m;
    initialize(n);
    
    for(int i = 0;i <m;++i){
        vector<int>v;   
        cin >> x >> y;
        
        combine(x,y);
        for(int i=0;i<MAX;i++){
            if(id[i]==i) v.push_back(size[i]);
        }
        sort(all(v));
        for(int i=0;i<v.size();i++) cout<<v[i]<<" ";
        cout<<endl;
    }
    return 0;
}
