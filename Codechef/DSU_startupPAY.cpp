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
    }
    else{
        id[p]=q; 
        size[q]+=size[p];
    }
}
vector<int> m[MAX];
int main(){
    
    int n; cin>>n;
    int ind;
    char ch;
    initialize(MAX);
    for(int i=2;i<=n;i++){
        cin>>ch;
        if(ch=='J'){
            cin>>ind;
            combine(ind,i);
        }
    }
    for(int i=1;i<=n;i++){
    	ind = findset(i);
    	m[ind].push_back(i);
    }
    int ans[n+5],len;
   	for(int i=1;i<=n;i++){
   		if(m[i].size() > 0){
   			len = m[i].size();
   			for(int j=0;j<len;j++)
   				ans[ m[i][j]] = len-j-1;
   		}
   	}
   	
    for(int i=1;i<=n;i++) cout<<ans[i]<<" ";
    return 0;
}
