#include <bits/stdc++.h>
#define all(x) x.begin(),x.end()
#define F first
#define S second

using namespace std;
typedef long long ll;
typedef vector<int> vi;
typedef pair<int,int> pii;


const int MAX = 1e5 + 5;
int id[MAX];
int sz[MAX];

multiset<int> sett;

void initialize(int n){
    for(int i = 1;i <=n;++i){   
         id[i] = i;
         sz[i]=1; sett.insert(1);
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
    
        sett.erase( sett.find(sz[p])); sett.erase( sett.find(sz[q]));
        id[q]=p; sz[p]+=sz[q];

        sz[q]=-1;
        sett.insert(sz[p]);
}
int main(){
    
    int n,q; scanf("%d %d",&n,&q);
    
    initialize(n);
    int x,y;

    for(int i=1;i<=q;i++){
        scanf("%d %d",&x,&y);
        combine(x,y);
        
        cout<< *sett.rbegin() - *sett.begin() <<"\n";
    }
    
    return 0;
}